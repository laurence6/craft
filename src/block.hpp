#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <GL/glew.h>

#include "render.hpp"

using namespace std;

static constexpr uint8_t
    block_face_left   = 0,
    block_face_right  = 1,
    block_face_front  = 2,
    block_face_back   = 3,
    block_face_bottom = 4,
    block_face_top    = 5;
static constexpr array<array<GLfloat, 2*3*3>, 6> gen_id_block_vertices() {
    constexpr GLfloat _min = 0.00f, _max = 0.01f;
    return {{
        [block_face_left] = {{
            _min, _min, _max,
            _min, _max, _max,
            _min, _max, _min,

            _min, _max, _min,
            _min, _min, _min,
            _min, _min, _max,
        }},
        [block_face_right] = {{
            _max, _max, _max,
            _max, _min, _max,
            _max, _min, _min,

            _max, _min, _min,
            _max, _max, _min,
            _max, _max, _max,
        }},
        [block_face_front] = {{
            _max, _min, _max,
            _min, _min, _max,
            _min, _min, _min,

            _min, _min, _min,
            _max, _min, _min,
            _max, _min, _max,
        }},
        [block_face_back] = {{
            _min, _max, _max,
            _max, _max, _max,
            _max, _max, _min,

            _max, _max, _min,
            _min, _max, _min,
            _min, _max, _max,
        }},
        [block_face_bottom] = {{
            _max, _min, _min,
            _min, _min, _min,
            _min, _max, _min,

            _min, _max, _min,
            _max, _max, _min,
            _max, _min, _min,
        }},
        [block_face_top] = {{
            _min, _max, _max,
            _min, _min, _max,
            _max, _min, _max,

            _max, _min, _max,
            _max, _max, _max,
            _min, _max, _max,
        }},
    }};
}
static constexpr array<array<GLfloat, 2*3*3>, 6> id_block_vertices = gen_id_block_vertices();

static constexpr array<GLfloat, 3*2*2> face_uv = {{
    1.f, 0.f,
    0.f, 0.f,
    0.f, 1.f,

    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
}};

class Block {
    friend class BlockChunk;

public:
    const int32_t x;
    const int32_t y;
    const uint8_t z;

private:
    const array<GLfloat, 6> tex;

protected:
    Block(int32_t x, int32_t y, uint8_t z, array<GLfloat, 6> tex) : x(x), y(y), z(z), tex(tex) {}

private:
    template<uint8_t f>
    void insert_face_vertices_uv(vector<GLfloat>& vertices, vector<GLfloat>& uv) {
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
};

static constexpr array<GLfloat, 6> dirt_block_tex = {{ 2, 2, 2, 2, 2, 2 }};
class DirtBlock : public Block {
public:
    DirtBlock(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, dirt_block_tex) {}
};

static constexpr array<GLfloat, 6> grass_block_tex = {{ 1, 1, 1, 1, 2, 0 }};
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

class BlockChunk {
    friend class BlockManager;

private:
    array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks = {};

    vector<GLfloat> vertices = {};
    vector<GLfloat> uv       = {};

public:
    const vector<GLfloat>& get_vertices() const {
        return vertices;
    }

    const vector<GLfloat>& get_uv() const {
        return uv;
    }

private:
    void add_block(Block* block) {
        _get_block(block->x, block->y, block->z) = block;
    }

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        return _get_block(x, y, z);
    }

    void update_vertices_uv(const uint64_t chunk_id) {
        vertices.clear();
        uv.clear();
        for (uint8_t z = 255; ; z--) {
            for (uint64_t x = 0; x < CHUNK_WIDTH; x++) {
                for (uint64_t y = 0; y < CHUNK_WIDTH; y++) {
                    Block* block = blocks[z][x][y];
                    if (block == nullptr) continue;
                    if (x == 0             || blocks[z][x-1][y] == nullptr) block->insert_face_vertices_uv<block_face_left  >(vertices, uv);
                    if (x == CHUNK_WIDTH-1 || blocks[z][x+1][y] == nullptr) block->insert_face_vertices_uv<block_face_right >(vertices, uv);
                    if (y == 0             || blocks[z][x][y-1] == nullptr) block->insert_face_vertices_uv<block_face_front >(vertices, uv);
                    if (y == CHUNK_WIDTH-1 || blocks[z][x][y+1] == nullptr) block->insert_face_vertices_uv<block_face_back  >(vertices, uv);
                    if (z == 0             || blocks[z-1][x][y] == nullptr) block->insert_face_vertices_uv<block_face_bottom>(vertices, uv);
                    if (z == 255           || blocks[z+1][x][y] == nullptr) block->insert_face_vertices_uv<block_face_top   >(vertices, uv);
                }
            }
            if (z == 0) break;
        }

        RenderManager::instance().upload_data_chunk(chunk_id, vertices, uv, GL_TRIANGLES);
    }

    Block*& _get_block(int32_t x, int32_t y, uint8_t z) {
        return blocks[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
    }
};

class BlockManager {
private:
    unordered_map<uint64_t, BlockChunk> chunks = {};
    unordered_set<uint64_t> chunks_need_update = {};

public:
    void add_block(Block* block) {
        uint64_t chunk_id = block_chunk_id(block->x, block->y);
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            chunk->second.add_block(block);
        } else {
            chunks.insert(make_pair(chunk_id, BlockChunk()));
            chunks.at(chunk_id).add_block(block);
            RenderManager::instance().add_chunk(chunk_id);
        }

        chunks_need_update.insert(chunk_id);
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

    void update_vertices_uv() {
        if (!chunks_need_update.empty()) {
            for (uint64_t chunk_id : chunks_need_update) {
                chunks.at(chunk_id).update_vertices_uv(chunk_id);
            }
            chunks_need_update.clear();
        }
    }

private:
    static uint64_t block_chunk_id(int32_t x, int32_t y) {
        uint64_t id = 0;
        id += ((static_cast<int64_t>(x) - numeric_limits<int32_t>::min()) & CHUNK_ID_MASK) << 32;
        id += ((static_cast<int64_t>(y) - numeric_limits<int32_t>::min()) & CHUNK_ID_MASK);
        return id;
    }
};

#endif
