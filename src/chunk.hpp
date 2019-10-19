#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <vector>

#include "block.hpp"

using namespace std;

/*
 * Chunk:
 *  16 * 16 * 256
 */

constexpr uint32_t CHUNK_WIDTH = 16;
constexpr uint64_t BLOCK_INDEX_MASK = 0x0000'000f, CHUNK_ID_MASK = 0xffff'fff0;

class ChunkID {
public:
    struct Hasher {
        size_t operator()(ChunkID const& chunk_id) const {
            uint64_t id = 0;
            id += static_cast<uint64_t>(chunk_id.x) << 32u;
            id += static_cast<uint64_t>(chunk_id.y);
            return id;
        }
    };

public:
    uint32_t x, y;

public:
    ChunkID(int32_t _x, int32_t _y) {
        x = static_cast<uint32_t>(_x) & CHUNK_ID_MASK;
        y = static_cast<uint32_t>(_y) & CHUNK_ID_MASK;
    }

    ChunkID add(int32_t dx, int32_t dy) const {
        return ChunkID { static_cast<int32_t>(x + CHUNK_WIDTH * dx), static_cast<int32_t>(y + CHUNK_WIDTH * dy) };
    }

    bool operator==(ChunkID const& o) const {
        return x == o.x && y == o.y;
    }
};

class ChunkVertices : private NonCopy<ChunkVertices> {
private:
    GLuint vao;
    GLuint vbo;
    size_t count = 0;

public:
    ChunkVertices();

    void upload_data(vector<BlockVertexData> const& data);

    void render() const;

    ~ChunkVertices();
};

class Chunk : private NonCopy<Chunk> {
public:
    const ChunkID chunk_id;

private:
    array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks {};
    array<array<array<  bool, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> opaque {};

    ChunkVertices chunk_vertices;

public:
    explicit Chunk(ChunkID chunk_id) : chunk_id(chunk_id) {
    }

    static vector<uint32_t> unload(Chunk*& chunk);

    void add_block(Block* block) {
        _get_block(block->x, block->y, block->z) = block;
        _get_opaque(block->x, block->y, block->z) = block->is_opaque();
    }

    void del_block(Block* block) {
        _get_block(block->x, block->y, block->z) = nullptr;
        _get_opaque(block->x, block->y, block->z) = false;
    }

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        return _get_block(x, y, z);
    }

    void update(array<Chunk const*, 4>&& adj_chunks);

    void render() const {
        chunk_vertices.render();
    }

private:
    Block*& _get_block(int32_t x, int32_t y, uint8_t z) {
        return blocks[z][static_cast<uint64_t>(x) & BLOCK_INDEX_MASK][static_cast<uint64_t>(y) & BLOCK_INDEX_MASK];
    }

    bool& _get_opaque(int32_t x, int32_t y, uint8_t z) {
        return opaque[z][static_cast<uint64_t>(x) & BLOCK_INDEX_MASK][static_cast<uint64_t>(y) & BLOCK_INDEX_MASK];
    }

    // 32 bits:
    //    x :  4,
    //    y :  4,
    //    z :  8,
    //   id : 10,
    //      :  6, (currently unused)
    static uint32_t marshal(Block const* block) {
        uint32_t v = 0;
        v += (static_cast<uint32_t>(block->x) & BLOCK_INDEX_MASK) << 28u;
        v += (static_cast<uint32_t>(block->y) & BLOCK_INDEX_MASK) << 24u;
        v += static_cast<uint32_t>(block->z) << 16u;
        v += static_cast<uint32_t>(block->id()) << 6u;
        return v;
    }

    static Block* unmarshal(ChunkID chunk_id, uint32_t block) {
        constexpr uint32_t X_MASK  = 0xf000'0000;
        constexpr uint32_t Y_MASK  = 0x0f00'0000;
        constexpr uint32_t Z_MASK  = 0x00ff'0000;
        constexpr uint32_t ID_MASK = 0x0000'ffc0;

        auto x = static_cast<int32_t>(((block & X_MASK) >> 28u) + chunk_id.x);
        auto y = static_cast<int32_t>(((block & Y_MASK) >> 24u) + chunk_id.y);
        auto z = static_cast<uint8_t>((block & Z_MASK) >> 16u);
        auto id = static_cast<uint16_t>((block & ID_MASK) >> 6u);

        return new_block(id, x, y, z);
    }
};

#endif
