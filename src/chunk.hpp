#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <tuple>
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

    ~ChunkVertices();

    void upload_data(vector<BlockVertexData> const& data);

    void render() const;
};

class Chunk : private NonCopy<Chunk> {
public:
    const ChunkID chunk_id;

private:
    array<array<array<Block, 256>, CHUNK_WIDTH>, CHUNK_WIDTH> blocks {};
    array<array<array< bool, 256>, CHUNK_WIDTH>, CHUNK_WIDTH> opaque {};

    ChunkVertices chunk_vertices;

public:
    explicit Chunk(ChunkID const& chunk_id);

    ~Chunk();

    void add_block(Block&& block) {
        auto [x, y, z] = internal_coord(block);
        opaque[x][y][z] = block.is_opaque();
        blocks[x][y][z] = move(block);
    }

    void del_block(Block const* block) {
        auto [x, y, z] = internal_coord(*block);
        opaque[x][y][z] = false;
        blocks[x][y][z].clear();
    }

    Block const* get_block(int32_t _x, int32_t _y, uint8_t _z) {
        auto [x, y, z] = internal_coord(_x, _y, _z);
        return &blocks[x][y][z];
    }

    void update(array<Chunk const*, 4>&& adj_chunks);

    void render() const {
        chunk_vertices.render();
    }

private:
    using internal_coord_t = tuple<uint16_t, uint16_t, uint8_t>;

    static internal_coord_t internal_coord(int32_t x, int32_t y, uint8_t z) {
        return { static_cast<uint64_t>(x) & BLOCK_INDEX_MASK, static_cast<uint64_t>(y) & BLOCK_INDEX_MASK, z };
    }

    static internal_coord_t internal_coord(Block const& block) {
        return internal_coord(block.x, block.y, block.z);
    }
};

#endif
