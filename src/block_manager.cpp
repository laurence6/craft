#include <algorithm>
#include <utility>

#include "block_manager.hpp"
#include "shader.hpp"
#include "util.hpp"

vector<uint32_t> Chunk::unload(Chunk* chunk) {
    vector<uint32_t> chunk_data = {};

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

    delete chunk;

    return chunk_data;
}

void Chunk::update_vertices(uint8_t flags, array<const Chunk*, 4> adj_chunks) {
#define INSERT_OUTER_SURFACE(S, XY, INDEX_SELF, INDEX_ADJ)                                                     \
    if ((flags & FACE_ ## S ## _BIT) != 0) {                                                                   \
        vd_outer[FACE_ ## S].clear();                                                                          \
        for (uint16_t z = 0; z < 256; z++) {                                                                   \
            for (uint16_t XY = 0; XY < CHUNK_WIDTH; XY++) {                                                    \
                Block* block = blocks[z]INDEX_SELF;                                                            \
                if (block != nullptr && block->has_six_faces()                                                 \
                        && (adj_chunks[FACE_ ## S] == nullptr || !adj_chunks[FACE_ ## S]->opaque[z]INDEX_ADJ)) { \
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

    chunk_vertices.upload_data(vertices);
}

void BlockManager::add_block(Block* block) {
    ChunkID chunk_id = ChunkID(block->x, block->y);

    Chunk* chunk;
    auto chunk_i = chunks.find(chunk_id);
    if (chunk_i != chunks.end()) {
        chunk = chunk_i->second;
    } else {
        chunk = new Chunk(chunk_id);
        chunks.insert(make_pair(chunk_id, chunk));
    }

    chunk->add_block(block);

    uint8_t flags = boarder_flags(block);
    add_chunk_need_update(chunk_id, flags);
    if (flags != 0) {
        if ((flags & FACE_LEFT_BIT) != 0) add_chunk_need_update(chunk_id.add(-1, 0), FACE_RIGHT_BIT);
        if ((flags & FACE_RIGHT_BIT) != 0) add_chunk_need_update(chunk_id.add( 1, 0), FACE_LEFT_BIT);
        if ((flags & FACE_FRONT_BIT) != 0) add_chunk_need_update(chunk_id.add(0, -1), FACE_BACK_BIT);
        if ((flags & FACE_BACK_BIT) != 0) add_chunk_need_update(chunk_id.add(0, 1), FACE_FRONT_BIT);
    }
}

void BlockManager::update_vertices() {
    if (!chunks_need_update.empty()) {
        for (auto const& chunk_u : chunks_need_update) {
            auto chunk_iter = chunks.find(chunk_u.first);
            if (chunk_iter != chunks.end()) {
                ChunkID chunk_id = ChunkID(chunk_u.first);
                chunk_iter->second->update_vertices(
                    chunk_u.second,
                    {{
                        get_chunk(chunk_id.add(-1, 0)),
                        get_chunk(chunk_id.add( 1, 0)),
                        get_chunk(chunk_id.add( 0,-1)),
                        get_chunk(chunk_id.add( 0, 1)),
                    }}
                );
            }
        }
        chunks_need_update.clear();
    }
}
