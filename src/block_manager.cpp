#include <algorithm>
#include <utility>

#include "block_manager.hpp"
#include "render.hpp"
#include "util.hpp"

void BlockVerticesArena::Proxy::upload_data(const vector<BlockVertexData>& vertices) {
    size_t n_ab = vertices.size() / ARENA_BLOCK_N_VERTICES;
    size_t rem = vertices.size() % ARENA_BLOCK_N_VERTICES;
    if (rem > 0) {
        n_ab += 1;
    } else {
        rem = ARENA_BLOCK_N_VERTICES;
    }

    arena->alloc_n_arena_blocks(arena_blocks, n_ab);

    if (n_ab > 0) {
        for (size_t i = 0; i < n_ab-1; i++) {
            arena_blocks[i]->count = ARENA_BLOCK_N_VERTICES;
            arena->upload_data(arena_blocks[i], &vertices[ARENA_BLOCK_N_VERTICES * i]);
        }
        arena_blocks[n_ab-1]->count = rem;
        arena->upload_data(arena_blocks[n_ab-1], &vertices[ARENA_BLOCK_N_VERTICES * (n_ab-1)]);
    }
}

void BlockVerticesArena::init() {
    for (size_t i = 0; i < ARENA_INIT; i++) {
        unused.push_back(new Block(ARENA_BLOCK_SIZE * length));
        length += 1;
    }

    vbo = gen_vbo();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, ARENA_BLOCK_SIZE * length, nullptr, GL_DYNAMIC_DRAW);

    vao = gen_vao();
    RenderManager::instance().blocks_vao = vao;
    update_vao();
}

void BlockVerticesArena::update_vertices() const {
    RenderManager& rm = RenderManager::instance();
    rm.blocks_first.clear();
    rm.blocks_count.clear();
    for (const Block* used_block : used) {
        rm.blocks_first.push_back(used_block->offset / sizeof(BlockVertexData));
        rm.blocks_count.push_back(used_block->count);
    }
}

void BlockVerticesArena::update_vao() const {
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertexData), (GLvoid *)0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(BlockVertexData), (GLvoid *)(3 * sizeof(GLfloat)));
}

void BlockVerticesArena::expand() {
    GLuint new_vbo = gen_vbo();
    size_t new_length = length;

    for (size_t i = 0; i < ARENA_GROWTH; i++) {
        unused.push_back(new Block(ARENA_BLOCK_SIZE * new_length));
        new_length += 1;
    }

    glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
    glBufferData(GL_ARRAY_BUFFER, ARENA_BLOCK_SIZE * new_length, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, vbo);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, ARENA_BLOCK_SIZE * length);

    glDeleteBuffers(1, &vbo);

    vbo = new_vbo;
    length = new_length;

    update_vao();
}

void BlockVerticesArena::alloc_n_arena_blocks(vector<Block*>& arena_blocks, size_t n) {
    if (n == arena_blocks.size()) {
        return;
    } else if (n < arena_blocks.size()) {
        size_t n_to_free = arena_blocks.size() - n;
        for (size_t i = 0; i < n_to_free; i++) {
            Block* arena_block = arena_blocks.back();
            arena_blocks.pop_back();

            arena_block->reset();

            auto it = find(used.begin(), used.end(), arena_block);
            swap(*it, used.back());
            used.pop_back();

            unused.push_back(arena_block);
        }
    } else {
        size_t n_to_alloc = n - arena_blocks.size();
        while(unused.size() < n_to_alloc) {
            expand();
        }

        for (size_t i = 0; i < n_to_alloc; i++) {
            Block* arena_block = unused.back();
            unused.pop_back();

            used.push_back(arena_block);

            arena_blocks.push_back(arena_block);
        }
    }
}

void BlockVerticesArena::upload_data(const Block* arena_block, const BlockVertexData* data) const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, arena_block->offset, sizeof(BlockVertexData) * arena_block->count, data);
}

vector<uint32_t> BlockManager::Chunk::unload(Chunk* chunk) {
    vector<uint32_t> chunk_data = {};
    chunk_data.reserve(CHUNK_WIDTH * CHUNK_WIDTH * 128);

    for (uint16_t z = 0; z < 256; z++) {
        for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
            for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                Block* block = chunk->blocks[z][x][y];
                if (block != nullptr) {
                    chunk_data.push_back(marshal(block));
                    delete block;
                }
            }
        }
    }

    chunk->arena_proxy.reset();
    delete chunk;

    chunk_data.shrink_to_fit();
    return chunk_data;
}

void BlockManager::Chunk::update_vertices(uint8_t flags, array<const Chunk*, 4> chunk_adj) {
#define INSERT_OUTER_SURFACE(S, XY, INDEX_SELF, INDEX_ADJ)                                                     \
    if ((flags & FACE_ ## S ## _BIT) != 0) {                                                                   \
        vd_outer[FACE_ ## S].clear();                                                                          \
        for (uint16_t z = 0; z < 256; z++) {                                                                   \
            for (uint16_t XY = 0; XY < CHUNK_WIDTH; XY++) {                                                    \
                Block* block = blocks[z]INDEX_SELF;                                                            \
                if (block != nullptr && block->has_six_faces()                                                 \
                        && (chunk_adj[FACE_ ## S] == nullptr || !chunk_adj[FACE_ ## S]->opaque[z]INDEX_ADJ)) { \
                    block->insert_face_vertices(vd_outer[FACE_ ## S], FACE_ ## S);                             \
                }                                                                                              \
            }                                                                                                  \
        }                                                                                                      \
    }                                                                                                          \

    INSERT_OUTER_SURFACE(LEFT,  y, [0][y], [CHUNK_WIDTH-1][y])
    INSERT_OUTER_SURFACE(RIGHT, y, [CHUNK_WIDTH-1][y], [0][y])
    INSERT_OUTER_SURFACE(FRONT, x, [x][0], [x][CHUNK_WIDTH-1])
    INSERT_OUTER_SURFACE(BACK,  x, [x][CHUNK_WIDTH-1], [x][0])

#undef INSERT_OUTER_SURFACE

    for (uint16_t z = 0; z < 256; z++) {
        for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
            for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                Block* block = blocks[z][x][y];
                if (block == nullptr) continue;

                if (block->has_six_faces()) {
                    if (x > 0             && !opaque[z][x-1][y]) block->insert_face_vertices(vd_inner, FACE_LEFT );
                    if (x < CHUNK_WIDTH-1 && !opaque[z][x+1][y]) block->insert_face_vertices(vd_inner, FACE_RIGHT);
                    if (y > 0             && !opaque[z][x][y-1]) block->insert_face_vertices(vd_inner, FACE_FRONT);
                    if (y < CHUNK_WIDTH-1 && !opaque[z][x][y+1]) block->insert_face_vertices(vd_inner, FACE_BACK );

                    if (z == 0            || !opaque[z-1][x][y]) block->insert_face_vertices(vd_inner, FACE_BOTTOM);
                    if (z == 255          || !opaque[z+1][x][y]) block->insert_face_vertices(vd_inner, FACE_TOP   );
                } else {
                    block->insert_face_vertices(vd_inner);
                }
            }
        }
    }

    size_t c = vd_inner.size();
    for (int i = 0; i < 4; i++) {
        c += vd_outer[i].size();
    }

    vector<BlockVertexData> vertices = {};
    vertices.reserve(c);

    vertices.insert(vertices.end(), vd_inner.begin(), vd_inner.end());
    for (int i = 0; i < 4; i++) {
        vertices.insert(vertices.end(), vd_outer[i].begin(), vd_outer[i].end());
    }

    arena_proxy.upload_data(vertices);
}

void BlockManager::add_block(Block* block) {
    ChunkID chunk_id = ChunkID(block->x, block->y);
    uint64_t _chunk_id = chunk_id.gen_chunk_id();

    Chunk* chunk;
    auto chunk_i = chunks.find(_chunk_id);
    if (chunk_i != chunks.end()) {
        chunk = chunk_i->second;
    } else {
        chunk = new Chunk(&arena);
        chunks.insert(make_pair(_chunk_id, chunk));
    }

    chunk->add_block(block);

    uint8_t flags = boarder_flags(block);
    add_chunk_need_update(_chunk_id, flags);
    if (flags != 0) {
        if ((flags & FACE_LEFT_BIT) != 0) add_chunk_need_update(chunk_id.update<-1, 0>().gen_chunk_id(), FACE_RIGHT_BIT);
        if ((flags & FACE_RIGHT_BIT) != 0) add_chunk_need_update(chunk_id.update< 1, 0>().gen_chunk_id(), FACE_LEFT_BIT);
        if ((flags & FACE_FRONT_BIT) != 0) add_chunk_need_update(chunk_id.update<0,-1>().gen_chunk_id(), FACE_BACK_BIT);
        if ((flags & FACE_BACK_BIT) != 0) add_chunk_need_update(chunk_id.update<0, 1>().gen_chunk_id(), FACE_FRONT_BIT);
    }
}

void BlockManager::update_vertices() {
    if (!chunks_need_update.empty()) {
        for (const auto& chunk_u : chunks_need_update) {
            auto chunk_iter = chunks.find(chunk_u.first);
            if (chunk_iter != chunks.end()) {
                ChunkID chunk_id = ChunkID(chunk_u.first);
                chunk_iter->second->update_vertices(
                    chunk_u.second,
                    {{
                        get_chunk(chunk_id.update<-1, 0>().gen_chunk_id()),
                        get_chunk(chunk_id.update< 1, 0>().gen_chunk_id()),
                        get_chunk(chunk_id.update< 0,-1>().gen_chunk_id()),
                        get_chunk(chunk_id.update< 0, 1>().gen_chunk_id()),
                    }}
                );
            }
        }
        chunks_need_update.clear();
        arena.update_vertices();
    }
}
