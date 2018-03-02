#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <algorithm>
#include <array>
#include <unordered_map>
#include <unordered_set>
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
        size_t const offset;
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

        void update_vertices(const Chunk* chunk_left, const Chunk* chunk_right, const Chunk* chunk_front, const Chunk* chunk_back) {
            vector<BlockVertexData> vertices = {};
            vertices.reserve(ARENA_BLOCK_N_VERTICES);

            for (uint16_t z = 0; z < 256; z++) {
                for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                    Block* block = blocks[z][0][y];
                    if (block != nullptr && block->has_six_faces() && (chunk_left == nullptr || !chunk_left->opaque[z][15][y])) block->insert_face_vertices(vertices, FACE_LEFT);
                }
            }

            for (uint16_t z = 0; z < 256; z++) {
                for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                    Block* block = blocks[z][15][y];
                    if (block != nullptr && block->has_six_faces() && (chunk_right == nullptr || !chunk_right->opaque[z][0][y])) block->insert_face_vertices(vertices, FACE_RIGHT);
                }
            }

            for (uint16_t z = 0; z < 256; z++) {
                for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
                    Block* block = blocks[z][x][0];
                    if (block != nullptr && block->has_six_faces() && (chunk_front == nullptr || !chunk_front->opaque[z][x][15])) block->insert_face_vertices(vertices, FACE_FRONT);
                }
            }

            for (uint16_t z = 0; z < 256; z++) {
                for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
                    Block* block = blocks[z][x][15];
                    if (block != nullptr && block->has_six_faces() && (chunk_back == nullptr || !chunk_back->opaque[z][x][0])) block->insert_face_vertices(vertices, FACE_BACK);
                }
            }

            for (uint16_t z = 0; z < 256; z++) {
                for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
                    for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                        Block* block = blocks[z][x][y];
                        if (block == nullptr) continue;

                        if (block->has_six_faces()) {
                            if (x > 0             && !opaque[z][x-1][y]) block->insert_face_vertices(vertices, FACE_LEFT );
                            if (x < CHUNK_WIDTH-1 && !opaque[z][x+1][y]) block->insert_face_vertices(vertices, FACE_RIGHT);
                            if (y > 0             && !opaque[z][x][y-1]) block->insert_face_vertices(vertices, FACE_FRONT);
                            if (y < CHUNK_WIDTH-1 && !opaque[z][x][y+1]) block->insert_face_vertices(vertices, FACE_BACK );

                            if (z == 0            || !opaque[z-1][x][y]) block->insert_face_vertices(vertices, FACE_BOTTOM);
                            if (z == 255          || !opaque[z+1][x][y]) block->insert_face_vertices(vertices, FACE_TOP   );
                        } else {
                            block->insert_face_vertices(vertices);
                        }
                    }
                }
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
    unordered_map<uint64_t, Chunk*> chunks     = {};
    unordered_set<uint64_t> chunks_need_update = {};

    BlockVerticesArena arena;

public:
    void init() {
        arena.init();
    }

    void add_block(Block* block) {
        uint64_t chunk_id = ChunkID(block->x, block->y).gen_chunk_id();
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            chunk->second->add_block(block);
        } else {
            chunks.insert(make_pair(chunk_id, new Chunk(&arena)));
            chunks.at(chunk_id)->add_block(block);
        }

        chunks_need_update.insert(chunk_id);
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

    void update_vertices() {
        if (!chunks_need_update.empty()) {
            for (uint64_t _chunk_id : chunks_need_update) {
                ChunkID chunk_id = ChunkID(_chunk_id);
                chunks.at(_chunk_id)->update_vertices(
                    get_chunk(chunk_id.update<-1, 0>().gen_chunk_id()),
                    get_chunk(chunk_id.update< 1, 0>().gen_chunk_id()),
                    get_chunk(chunk_id.update< 0,-1>().gen_chunk_id()),
                    get_chunk(chunk_id.update< 0, 1>().gen_chunk_id())
                );
            }
            chunks_need_update.clear();
            arena.update_vertices();
        }
    }
};

#endif
