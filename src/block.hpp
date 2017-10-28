#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <limits>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

using namespace std;

static const array<array<GLfloat, 3*3*2>, 6> id_block_vertices = {{
    {{ // top
        0.01f, 0.00f, 0.01f,
        0.00f, 0.00f, 0.01f,
        0.00f, 0.01f, 0.01f,

        0.01f, 0.00f, 0.01f,
        0.01f, 0.01f, 0.01f,
        0.00f, 0.01f, 0.01f,
    }},

    {{ // front
        0.01f, 0.00f, 0.01f,
        0.00f, 0.00f, 0.01f,
        0.00f, 0.00f, 0.00f,

        0.01f, 0.00f, 0.01f,
        0.01f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f,
    }},

    {{ // left
        0.00f, 0.00f, 0.01f,
        0.00f, 0.01f, 0.01f,
        0.00f, 0.01f, 0.00f,

        0.00f, 0.00f, 0.01f,
        0.00f, 0.00f, 0.00f,
        0.00f, 0.01f, 0.00f,
    }},

    {{ // back
        0.00f, 0.01f, 0.01f,
        0.01f, 0.01f, 0.01f,
        0.01f, 0.01f, 0.00f,

        0.00f, 0.01f, 0.01f,
        0.00f, 0.01f, 0.00f,
        0.01f, 0.01f, 0.00f,
    }},

    {{ // right
        0.01f, 0.01f, 0.01f,
        0.01f, 0.00f, 0.01f,
        0.01f, 0.00f, 0.00f,

        0.01f, 0.01f, 0.01f,
        0.01f, 0.01f, 0.00f,
        0.01f, 0.00f, 0.00f,
    }},

    {{ // bottom
        0.01f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f,
        0.00f, 0.01f, 0.00f,

        0.01f, 0.00f, 0.00f,
        0.01f, 0.01f, 0.00f,
        0.00f, 0.01f, 0.00f,
    }},
}};

static const array<GLfloat, 3*2*2> face_uv = {{
    1.0, 0.0,
    0.0, 0.0,
    0.0, 1.0,

    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
}};

class BlockManager;

class Block {
public:
    inline void update_vertices_uv(BlockManager& blocks);

    inline void insert_face_vertices_uv(const uint8_t f) {
        for (uint8_t i = 0; i < 6; i++) {
            vertices.push_back(id_block_vertices[f][i*3+0] + static_cast<GLfloat>(x) / 100.f);
            vertices.push_back(id_block_vertices[f][i*3+1] + static_cast<GLfloat>(y) / 100.f);
            vertices.push_back(id_block_vertices[f][i*3+2] + static_cast<GLfloat>(z) / 100.f);
        }
        for (uint8_t i = 0; i < 6; i++) {
            uv.push_back(face_uv[i*2+0]);
            uv.push_back(face_uv[i*2+1]);
            uv.push_back(tex[f]);
        }
    }

    const int32_t x;
    const int32_t y;
    const uint8_t z;
    const array<GLfloat, 6> tex;

    vector<GLfloat> vertices;
    vector<GLfloat> uv;

protected:
    Block(int32_t x, int32_t y, uint8_t z, array<GLfloat, 6> tex) : x(x), y(y), z(z), tex(tex) {}
};

static const array<GLfloat, 6> dirt_block_tex = {{ 2, 2, 2, 2, 2, 2 }};
class DirtBlock : public Block {
public:
    DirtBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, dirt_block_tex) {}
};

static const array<GLfloat, 6> grass_block_tex = {{ 0, 1, 1, 1, 1, 2 }};
class GrassBlock : public Block {
public:
    GrassBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, grass_block_tex) {}
};

static const array<GLfloat, 6> stone_block_tex = {{ 3, 3, 3, 3, 3, 3 }};
class StoneBlock : public Block {
public:
    StoneBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, stone_block_tex) {}
};

/*
 * Chunk:
 *  16 * 16 * 256
 */

class BlockChunk {
public:
    inline void add_block(Block* block) {
        this->block(block->x, block->y, block->z) = block;
    }

    inline Block* get_block(int32_t x, int32_t y, uint8_t z) {
        return block(x, y, z);
    }

private:
    inline Block*& block(int32_t x, int32_t y, uint8_t z) {
        return blocks[z][x & 0xf][y & 0xf];
    }

    array<array<array<Block*, 16>, 16>, 256> blocks;
};

class BlockManager {
public:
    inline void add_block(Block* block) {
        uint64_t chunk_id = block_chunk_id(block->x, block->y);
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            chunk->second.add_block(block);
        } else {
            BlockChunk chunk = BlockChunk();
            chunk.add_block(block);
            chunks[chunk_id] = move(chunk);
        }
    }

    inline Block* get_block(int32_t x, int32_t y, uint8_t z) {
        uint64_t chunk_id = block_chunk_id(x, y);
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second.get_block(x, y, z);
        } else {
            return nullptr;
        }
    }

private:
    static inline uint64_t block_chunk_id(int32_t x, int32_t y) {
        uint64_t id = 0;
        id += (static_cast<uint64_t>(x) & 0xfffffff0) << 32;
        id += (static_cast<uint64_t>(y) & 0xfffffff0);
        return id;
    }

    unordered_map<uint64_t, BlockChunk> chunks;
};

inline void Block::update_vertices_uv(BlockManager& blocks) {
    vertices.clear();
    uv.clear();
    if (blocks.get_block(x-1, y, z) == nullptr) insert_face_vertices_uv(2);
    if (blocks.get_block(x+1, y, z) == nullptr) insert_face_vertices_uv(4);
    if (blocks.get_block(x, y-1, z) == nullptr) insert_face_vertices_uv(1);
    if (blocks.get_block(x, y+1, z) == nullptr) insert_face_vertices_uv(3);
    if (z == numeric_limits<uint8_t>::max() ||
            blocks.get_block(x, y, z+1) == nullptr) insert_face_vertices_uv(0);
    if (z == numeric_limits<uint8_t>::min() ||
            blocks.get_block(x, y, z-1) == nullptr) insert_face_vertices_uv(5);
}

#endif
