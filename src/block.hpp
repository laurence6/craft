#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <GL/glew.h>

#include "config.hpp"
#include "render.hpp"

using namespace std;

static constexpr GLfloat _min = 0.00f, _max = 0.01f;
static constexpr array<array<GLfloat, 2*3*3>, 6> id_block_vertices = {{
    [BLOCK_FACE_LEFT] = {{
        _min, _min, _max,
        _min, _max, _max,
        _min, _max, _min,

        _min, _max, _min,
        _min, _min, _min,
        _min, _min, _max,
    }},
    [BLOCK_FACE_RIGHT] = {{
        _max, _max, _max,
        _max, _min, _max,
        _max, _min, _min,

        _max, _min, _min,
        _max, _max, _min,
        _max, _max, _max,
    }},
    [BLOCK_FACE_FRONT] = {{
        _max, _min, _max,
        _min, _min, _max,
        _min, _min, _min,

        _min, _min, _min,
        _max, _min, _min,
        _max, _min, _max,
    }},
    [BLOCK_FACE_BACK] = {{
        _min, _max, _max,
        _max, _max, _max,
        _max, _max, _min,

        _max, _max, _min,
        _min, _max, _min,
        _min, _max, _max,
    }},
    [BLOCK_FACE_BOTTOM] = {{
        _max, _min, _min,
        _min, _min, _min,
        _min, _max, _min,

        _min, _max, _min,
        _max, _max, _min,
        _max, _min, _min,
    }},
    [BLOCK_FACE_TOP] = {{
        _min, _max, _max,
        _min, _min, _max,
        _max, _min, _max,

        _max, _min, _max,
        _max, _max, _max,
        _min, _max, _max,
    }},
}};
static constexpr array<GLfloat, 2*2*3*3> tf_block_vertices = {{
    _max, _min, _max,
    _min, _max, _max,
    _min, _max, _min,

    _min, _max, _min,
    _max, _min, _min,
    _max, _min, _max,

    _max, _max, _max,
    _min, _min, _max,
    _min, _min, _min,

    _min, _min, _min,
    _max, _max, _min,
    _max, _max, _max,
}};

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

public:
    static bool is_opaque(const Block* block) {
        return block != nullptr && block->is_opaque();
    }

    virtual bool is_opaque() const = 0;

protected:
    Block(int32_t x, int32_t y, uint8_t z) : x(x), y(y), z(z) {}

private:
    virtual bool six_faces() const = 0;

    virtual void insert_face_vertices(vector<GLfloat>&, uint8_t) const {}
    virtual void insert_face_vertices(vector<GLfloat>&) const {}
};

class OpaqueBlock : public Block {
private:
    const array<GLfloat, 6> tex;

public:
    OpaqueBlock(int32_t x, int32_t y, int32_t z, array<GLfloat, 6> tex) : Block(x, y, z), tex(tex) {}

    bool is_opaque() const override {
        return true;
    }

private:
    bool six_faces() const override {
        return true;
    }

    void insert_face_vertices(vector<GLfloat>& vertices, uint8_t f) const override {
        for (uint8_t i = 0; i < 6; i++) {
            vertices.push_back(id_block_vertices[f][i*3+0] + static_cast<GLfloat>(x) / 100.f);
            vertices.push_back(id_block_vertices[f][i*3+1] + static_cast<GLfloat>(y) / 100.f);
            vertices.push_back(id_block_vertices[f][i*3+2] + static_cast<GLfloat>(z) / 100.f);
            vertices.push_back(face_uv[i*2+0]);
            vertices.push_back(face_uv[i*2+1]);
            vertices.push_back(tex[f]);
        }
    }
};

class GrassBlock : public Block {
private:
    const GLfloat tex;

public:
    GrassBlock(int32_t x, int32_t y, uint8_t z, GLfloat tex) : Block(x, y, z), tex(tex) {}

    bool is_opaque() const override {
        return false;
    }

private:
    bool six_faces() const override {
        return false;
    }

    void insert_face_vertices(vector<GLfloat>& vertices) const override {
        for (uint8_t f = 0; f < 2; f++) {
            for (uint8_t i = 0; i < 6; i++) {
                vertices.push_back(tf_block_vertices[f*18+i*3+0] + static_cast<GLfloat>(x) / 100.f);
                vertices.push_back(tf_block_vertices[f*18+i*3+1] + static_cast<GLfloat>(y) / 100.f);
                vertices.push_back(tf_block_vertices[f*18+i*3+2] + static_cast<GLfloat>(z) / 100.f);
                vertices.push_back(face_uv[i*2+0]);
                vertices.push_back(face_uv[i*2+1]);
                vertices.push_back(tex);
            }
        }
    }
};

/*
 * Chunk:
 *  16 * 16 * 256
 */

static constexpr uint64_t CHUNK_WIDTH = 16;
static constexpr uint64_t BLOCK_INDEX_MASK = 0x0000'000f;
static constexpr uint64_t CHUNK_ID_MASK    = 0xffff'fff0;

static uint64_t block_chunk_id(int32_t x, int32_t y) {
    uint64_t id = 0;
    id += ((static_cast<int64_t>(x) - numeric_limits<int32_t>::min()) & CHUNK_ID_MASK) << 32;
    id += ((static_cast<int64_t>(y) - numeric_limits<int32_t>::min()) & CHUNK_ID_MASK);
    return id;
}

class BlockChunk {
    friend class BlockManager;

private:
    array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks = {};
    array<array<array<  bool, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> opaque = {};

    vector<GLfloat> vertices = {};

public:
    const vector<GLfloat>& get_vertices() const {
        return vertices;
    }

private:
    void add_block(Block* block) {
        _get_block(block->x, block->y, block->z) = block;
        _get_opaque(block->x, block->y, block->z) = Block::is_opaque(block);
    }

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        return _get_block(x, y, z);
    }

    Block*& _get_block(int32_t x, int32_t y, uint8_t z) {
        return blocks[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
    }

    bool& _get_opaque(int32_t x, int32_t y, uint8_t z) {
        return opaque[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
    }

    void update_vertices(uint64_t chunk_id, const BlockChunk* chunk_left, const BlockChunk* chunk_right, const BlockChunk* chunk_front, const BlockChunk* chunk_back) {
        vertices.clear();

        for (uint16_t z = 0; z < 256; z++) {
            for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                Block* block = blocks[z][0][y];
                if (block != nullptr && block->six_faces() && (chunk_left == nullptr || !chunk_left->opaque[z][15][y])) block->insert_face_vertices(vertices, BLOCK_FACE_LEFT);
            }
        }

        for (uint16_t z = 0; z < 256; z++) {
            for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                Block* block = blocks[z][15][y];
                if (block != nullptr && block->six_faces() && (chunk_right == nullptr || !chunk_right->opaque[z][0][y])) block->insert_face_vertices(vertices, BLOCK_FACE_RIGHT);
            }
        }

        for (uint16_t z = 0; z < 256; z++) {
            for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
                Block* block = blocks[z][x][0];
                if (block != nullptr && block->six_faces() && (chunk_front == nullptr || !chunk_front->opaque[z][x][15])) block->insert_face_vertices(vertices, BLOCK_FACE_FRONT);
            }
        }

        for (uint16_t z = 0; z < 256; z++) {
            for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
                Block* block = blocks[z][x][15];
                if (block != nullptr && block->six_faces() && (chunk_back == nullptr || !chunk_back->opaque[z][x][0])) block->insert_face_vertices(vertices, BLOCK_FACE_BACK);
            }
        }

        for (uint16_t z = 0; z < 256; z++) {
            for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
                for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                    Block* block = blocks[z][x][y];
                    if (block == nullptr) continue;

                    if (block->six_faces()) {
                        if (x > 0             && !opaque[z][x-1][y]) block->insert_face_vertices(vertices, BLOCK_FACE_LEFT );
                        if (x < CHUNK_WIDTH-1 && !opaque[z][x+1][y]) block->insert_face_vertices(vertices, BLOCK_FACE_RIGHT);
                        if (y > 0             && !opaque[z][x][y-1]) block->insert_face_vertices(vertices, BLOCK_FACE_FRONT);
                        if (y < CHUNK_WIDTH-1 && !opaque[z][x][y+1]) block->insert_face_vertices(vertices, BLOCK_FACE_BACK );

                        if (z == 0            || !opaque[z-1][x][y]) block->insert_face_vertices(vertices, BLOCK_FACE_BOTTOM);
                        if (z == 255          || !opaque[z+1][x][y]) block->insert_face_vertices(vertices, BLOCK_FACE_TOP   );
                    } else {
                        block->insert_face_vertices(vertices);
                    }
                }
            }
        }

        RenderManager::instance().upload_data_chunk(chunk_id, vertices, GL_TRIANGLES, vertices.size()/6);
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

    const BlockChunk* get_chunk(uint64_t chunk_id) {
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return &chunk->second;
        } else {
            return nullptr;
        }
    }

    void update_vertices() {
        if (!chunks_need_update.empty()) {
            for (uint64_t chunk_id : chunks_need_update) {
                chunks.at(chunk_id).update_vertices(chunk_id,
                    get_chunk(chunk_id-(CHUNK_WIDTH<<32)),
                    get_chunk(chunk_id+(CHUNK_WIDTH<<32)),
                    get_chunk(chunk_id-(CHUNK_WIDTH    )),
                    get_chunk(chunk_id+(CHUNK_WIDTH    ))
                );
            }
            chunks_need_update.clear();
        }
    }
};

#endif
