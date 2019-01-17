#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <array>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"

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

        void upload_data(const vector<BlockVertexData>& vertices);

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
    void init();

    void update_vertices() const;

private:
    void update_vao() const;

    void expand();

    void alloc_n_arena_blocks(vector<Block*>& arena_blocks, size_t n);

    void upload_data(const Block* arena_block, const BlockVertexData* data) const;
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

        static vector<uint32_t> unload(Chunk* chunk);

        void add_block(Block* block) {
            _get_block(block->x, block->y, block->z) = block;
            _get_opaque(block->x, block->y, block->z) = Block::is_opaque(block);
        }

        Block* get_block(int32_t x, int32_t y, uint8_t z) {
            return _get_block(x, y, z);
        }

        void update_vertices(uint8_t flags, array<const Chunk*, 4> chunk_adj);

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

    void add_block(Block* block);

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

    void update_vertices();

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