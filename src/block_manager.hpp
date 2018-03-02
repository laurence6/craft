#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <algorithm>
#include <array>
#include <unordered_map>
#include <utility>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "render.hpp"
#include "util.hpp"

using namespace std;

constexpr size_t ARENA_BLOCK_SIZE = ARENA_BLOCK_N_VERTICES * sizeof(BlockVertexData);

class BlockVerticesArena : private NonCopy<BlockVerticesArena> {
private:
    class Block : private NonCopy<Block> {
    public:
        const size_t offset;
        size_t count = 0;

    public:
        Block(size_t offset) : offset(offset) {}

        void reset() {
            count = 0;
        }
    };

public:
    class Proxy : private NonCopy<Proxy> {
    private:
        BlockVerticesArena* const arena;
        vector<BlockVerticesArena::Block*> arena_blocks = {};

    public:
        Proxy(BlockVerticesArena* arena) : arena(arena) {}

        Proxy(Proxy&&) = default;

        void upload_data(const vector<BlockVertexData>& vertices) {
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

        void reset() {
            arena->alloc_n_arena_blocks(arena_blocks, 0);
        }
    };

private:
    GLuint         vao;
    GLuint         vbo;
    size_t         length = 0;
    vector<Block*> unused = {};
    vector<Block*> used   = {};

public:
    void init() {
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

    void update_vertices() const {
        RenderManager& rm = RenderManager::instance();
        rm.blocks_first.clear();
        rm.blocks_count.clear();
        for (const Block* used_block : used) {
            rm.blocks_first.push_back(used_block->offset / sizeof(BlockVertexData));
            rm.blocks_count.push_back(used_block->count);
        }
    }

private:
    void update_vao() const {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertexData), (GLvoid *)0);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(BlockVertexData), (GLvoid *)(3 * sizeof(GLfloat)));
    }

    void expand() {
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

    void alloc_n_arena_blocks(vector<Block*>& arena_blocks, size_t n) {
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

    void upload_data(const Block* arena_block, const BlockVertexData* data) const {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, arena_block->offset, sizeof(BlockVertexData) * arena_block->count, data);
    }
};

/*
 * Chunk:
 *  16 * 16 * 256
 */

constexpr uint64_t
    CHUNK_WIDTH      = 16,
    BLOCK_INDEX_MASK = 0x0000'000f,
    CHUNK_ID_MASK    = 0xffff'fff0;

class BlockManager : private NonCopy<BlockManager> {
private:
    class ChunkID {
    public:
        uint32_t x, y;

    public:
        ChunkID(int32_t _x, int32_t _y) {
            x = static_cast<uint32_t>(_x) & CHUNK_ID_MASK;
            y = static_cast<uint32_t>(_y) & CHUNK_ID_MASK;
        }

        ChunkID(uint64_t chunk_id) {
            x = chunk_id >> 32;
            y = chunk_id & 0xffff'ffff;
        }

        template<int32_t dx, int32_t dy>
        ChunkID update() const {
            ChunkID chunk_id;
            chunk_id.x = x + CHUNK_WIDTH * dx;
            chunk_id.y = y + CHUNK_WIDTH * dy;
            return chunk_id;
        }

        uint64_t gen_chunk_id() const {
            uint64_t id = 0;
            id += static_cast<uint64_t>(x) << 32;
            id += static_cast<uint64_t>(y);
            return id;
        }

    private:
        ChunkID() = default;
    };

    class Chunk : private NonCopy<Chunk> {
    private:
        array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks = {};
        array<array<array<  bool, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> opaque = {};

        array<vector<BlockVertexData>, 4> vd_outer = {}; // four sides of a chunk
        vector<BlockVertexData>           vd_inner = {};

        BlockVerticesArena::Proxy arena_proxy;

    public:
        Chunk(BlockVerticesArena* arena) : arena_proxy(BlockVerticesArena::Proxy(arena)) {}

        Chunk(BlockVerticesArena* arena, uint64_t chunk_id, const vector<uint32_t>& blocks) : Chunk(arena) {
            for (uint32_t b : blocks) {
                add_block(unmarshal(chunk_id, b));
            }
        }

        static vector<uint32_t> unload(Chunk* chunk) {
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

        void add_block(Block* block) {
            _get_block(block->x, block->y, block->z) = block;
            _get_opaque(block->x, block->y, block->z) = Block::is_opaque(block);
        }

        Block* get_block(int32_t x, int32_t y, uint8_t z) {
            return _get_block(x, y, z);
        }

        void update_vertices(uint8_t flags, array<const Chunk*, 4> chunk_adj) {
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

    private:
        Block*& _get_block(int32_t x, int32_t y, uint8_t z) {
            return blocks[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
        }

        bool& _get_opaque(int32_t x, int32_t y, uint8_t z) {
            return opaque[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
        }

        // 32 bits:
        //    x :  4,
        //    y :  4,
        //    z :  8,
        //   id : 10,
        //      :  6, (currently unused)
        static uint32_t marshal(const Block* block) {
            uint32_t v = 0;
            v += (static_cast<uint32_t>(block->x) & BLOCK_INDEX_MASK) << 28;
            v += (static_cast<uint32_t>(block->y) & BLOCK_INDEX_MASK) << 24;
            v += static_cast<uint32_t>(block->z) << 16;
            v += static_cast<uint32_t>(block->id()) << 6;
            return v;
        }

        static Block* unmarshal(uint64_t _chunk_id, uint32_t block) {
            constexpr uint32_t
                X_MASK  = 0xf000'0000,
                Y_MASK  = 0x0f00'0000,
                Z_MASK  = 0x00ff'0000,
                ID_MASK = 0x0000'ffc0;

            ChunkID chunk_id = ChunkID(_chunk_id);

            int32_t x = static_cast<int32_t>(((block & X_MASK) >> 28) + chunk_id.x);
            int32_t y = static_cast<int32_t>(((block & Y_MASK) >> 24) + chunk_id.y);
            uint8_t z = static_cast<uint8_t>((block & Z_MASK) >> 16);
            uint16_t id = static_cast<uint16_t>((block & ID_MASK) >> 6);

            return new_block(id, x, y, z);
        }
    };

private:
    unordered_map<uint64_t, Chunk*>  chunks             = {};
    unordered_map<uint64_t, uint8_t> chunks_need_update = {};

    BlockVerticesArena arena;

public:
    void init() {
        arena.init();
    }

    void add_block(Block* block) {
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

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        uint64_t chunk_id = ChunkID(x, y).gen_chunk_id();
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second->get_block(x, y, z);
        } else {
            return nullptr;
        }
    }

    const Chunk* get_chunk(uint64_t chunk_id) const {
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second;
        } else {
            return nullptr;
        }
    }

    void add_chunk_need_update(uint64_t chunk_id, uint8_t flags) {
        auto chunk_u_iter = chunks_need_update.find(chunk_id);
        if (chunk_u_iter != chunks_need_update.end()) {
            chunk_u_iter->second |= flags;
        } else {
            chunks_need_update.insert(make_pair(chunk_id, flags));
        }
    }

    void update_vertices() {
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

private:
    static uint8_t boarder_flags(const Block* block) {
        const uint8_t
            x = block->x & BLOCK_INDEX_MASK,
            y = block->y & BLOCK_INDEX_MASK;

        uint8_t flags = 0;

        if (x == 0)             flags |= FACE_LEFT_BIT;
        if (x == CHUNK_WIDTH-1) flags |= FACE_RIGHT_BIT;
        if (y == 0)             flags |= FACE_FRONT_BIT;
        if (y == CHUNK_WIDTH-1) flags |= FACE_BACK_BIT;

        return flags;
    }
};

#endif
