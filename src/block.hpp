#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <GL/glew.h>

using namespace std;

static constexpr array<array<GLfloat, 3*3*2>, 6> id_block_vertices = {{
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

static constexpr array<GLfloat, 3*2*2> face_uv = {{
    1.f, 0.f,
    0.f, 0.f,
    0.f, 1.f,

    1.f, 0.f,
    1.f, 1.f,
    0.f, 1.f,
}};

class BlockManager;

class Block {
public:
    template<bool update_neighbors>
    inline void update_vertices_uv(BlockManager* bm);

    void insert_face_vertices_uv(const uint8_t f) {
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

    const vector<GLfloat>& get_vertices() const {
        return vertices;
    }

    const vector<GLfloat>& get_uv() const {
        return uv;
    }

    const int32_t x;
    const int32_t y;
    const uint8_t z;

protected:
    Block(int32_t x, int32_t y, uint8_t z, array<GLfloat, 6> tex) : x(x), y(y), z(z), tex(tex) {}

    const array<GLfloat, 6> tex;

    vector<GLfloat> vertices = {};
    vector<GLfloat> uv       = {};
};

class VoidBlock : public Block {
public:
    VoidBlock() : Block(0, 0, 0, {{ 0, 0, 0, 0, 0, 0 }}) {}
};

static constexpr array<GLfloat, 6> dirt_block_tex = {{ 2, 2, 2, 2, 2, 2 }};
class DirtBlock : public Block {
public:
    DirtBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, dirt_block_tex) {}
};

static constexpr array<GLfloat, 6> grass_block_tex = {{ 0, 1, 1, 1, 1, 2 }};
class GrassBlock : public Block {
public:
    GrassBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, grass_block_tex) {}
};

static constexpr array<GLfloat, 6> stone_block_tex = {{ 3, 3, 3, 3, 3, 3 }};
class StoneBlock : public Block {
public:
    StoneBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, stone_block_tex) {}
};

/*
 * Chunk:
 *  16 * 16 * 256
 */

static constexpr uint64_t CHUNK_WIDTH_DIGITS = 4;
static constexpr uint64_t CHUNK_WIDTH = 1 << CHUNK_WIDTH_DIGITS;
static constexpr uint64_t BLOCK_INDEX_MASK = CHUNK_WIDTH - 1;
static constexpr uint64_t CHUNK_ID_MASK = 0xffff'ffff ^ BLOCK_INDEX_MASK;

class BlockManager {
private:
    class BlockChunk {
    public:
        void add_block(Block* block) {
            _get_block(block->x, block->y, block->z) = block;
        }

        Block* get_block(int32_t x, int32_t y, uint8_t z) {
            return _get_block(x, y, z);
        }

    private:
        Block*& _get_block(int32_t x, int32_t y, uint8_t z) {
            return blocks[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
        }

        array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks = {};
    };

public:
    void add_block(Block* block) {
        uint64_t chunk_id = block_chunk_id(block->x, block->y);
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            chunk->second.add_block(block);
        } else {
            BlockChunk chunk = BlockChunk();
            chunk.add_block(block);
            chunks[chunk_id] = move(chunk);
        }

        blocks.insert(block);

        block->update_vertices_uv<true>(this);
    }

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        uint64_t chunk_id = block_chunk_id(x, y);
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second.get_block(x, y, z);
        } else {
            return nullptr;
        }
    }

    const unordered_set<Block*>& get_blocks() const {
        return blocks;
    }

private:
    static uint64_t block_chunk_id(int32_t x, int32_t y) {
        uint64_t id = 0;
        id += ((static_cast<int64_t>(x) - numeric_limits<int32_t>::min()) & CHUNK_ID_MASK) << 32;
        id += ((static_cast<int64_t>(y) - numeric_limits<int32_t>::min()) & CHUNK_ID_MASK);
        return id;
    }

    unordered_map<uint64_t, BlockChunk> chunks = {};
    unordered_set<Block*> blocks = {};
};

Block* const void_block = new VoidBlock();

template<bool update_neighbors>
inline void Block::update_vertices_uv(BlockManager* bm) {
    vertices.clear();
    uv.clear();

    Block* neighbors[6] = {
        (z < numeric_limits<uint8_t>::max()) ? bm->get_block(x, y, z+1) : void_block,
        bm->get_block(x, y-1, z),
        bm->get_block(x-1, y, z),
        bm->get_block(x, y+1, z),
        bm->get_block(x+1, y, z),
        (z > numeric_limits<uint8_t>::min()) ? bm->get_block(x, y, z-1) : void_block,
    };

    for (uint8_t i = 0; i < 6; i++) {
        if (neighbors[i] == nullptr || neighbors[i] == void_block) {
            insert_face_vertices_uv(i);
        } else if (update_neighbors) {
            neighbors[i]->update_vertices_uv<false>(bm);
        }
    }
}

#endif
