#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <algorithm>
#include <array>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <GL/glew.h>

#include "config.hpp"
#include "render.hpp"
#include "util.hpp"

using namespace std;

constexpr GLfloat _min = 0.00f, _max = 0.01f;
constexpr array<array<GLfloat, 2*3*3>, 6> id_block_vertices = {{
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
constexpr array<GLfloat, 2*2*3*3> tf_block_vertices = {{
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

constexpr array<GLfloat, 2*2*3> face_uv = {{
    1.f, 0.f,
    0.f, 0.f,
    0.f, 1.f,

    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
}};

constexpr array<array<GLfloat, 3>, 6> face_normal = {{
    [BLOCK_FACE_LEFT  ] = {{-0.4f, 0.f, 0.9f }},
    [BLOCK_FACE_RIGHT ] = {{ 0.4f, 0.f, 0.9f }},
    [BLOCK_FACE_FRONT ] = {{ 0.f, 0.f, 1.f }},
    [BLOCK_FACE_BACK  ] = {{ 0.f, 0.f, 1.f }},
    [BLOCK_FACE_BOTTOM] = {{ 0.f, 0.f,-1.f }},
    [BLOCK_FACE_TOP   ] = {{ 0.f, 0.f, 1.f }},
}};

struct BlockVertexData {
    GLfloat        x,        y,        z;
    GLfloat     uv_x,     uv_y,     uv_z;
    GLfloat normal_x, normal_y, normal_z;
};

class Block : private NonCopy<Block> {
    friend class BlockManager;

public:
    const int32_t x;
    const int32_t y;
    const uint8_t z;

protected:
    Block(int32_t x, int32_t y, uint8_t z) : x(x), y(y), z(z) {}

private:
    static bool is_opaque(const Block* block) {
        return block != nullptr && block->is_opaque();
    }

    virtual bool is_opaque() const = 0;

    virtual bool six_faces() const = 0;

    virtual void insert_face_vertices(vector<BlockVertexData>&, uint8_t) const {}
    virtual void insert_face_vertices(vector<BlockVertexData>&) const {}
};

class OpaqueBlock : public Block {
private:
    const array<GLfloat, 6> tex;

public:
    OpaqueBlock(int32_t x, int32_t y, int32_t z, array<GLfloat, 6> tex) : Block(x, y, z), tex(tex) {}

private:
    bool is_opaque() const override {
        return true;
    }

    bool six_faces() const override {
        return true;
    }

    void insert_face_vertices(vector<BlockVertexData>& vertices, uint8_t f) const override {
        for (uint8_t i = 0; i < 6; i++) {
            vertices.push_back(BlockVertexData {
                id_block_vertices[f][i*3+0] + static_cast<GLfloat>(x) / 100.f,
                id_block_vertices[f][i*3+1] + static_cast<GLfloat>(y) / 100.f,
                id_block_vertices[f][i*3+2] + static_cast<GLfloat>(z) / 100.f,
                face_uv[i*2+0],
                face_uv[i*2+1],
                tex[f],
                face_normal[f][0],
                face_normal[f][1],
                face_normal[f][2],
            });
        }
    }
};

class GrassBlock : public Block {
private:
    const GLfloat tex;

public:
    GrassBlock(int32_t x, int32_t y, uint8_t z, GLfloat tex) : Block(x, y, z), tex(tex) {}

private:
    bool is_opaque() const override {
        return false;
    }

    bool six_faces() const override {
        return false;
    }

    void insert_face_vertices(vector<BlockVertexData>& vertices) const override {
        for (uint8_t f = 0; f < 2; f++) {
            for (uint8_t i = 0; i < 6; i++) {
                vertices.push_back(BlockVertexData {
                    tf_block_vertices[f*18+i*3+0] + static_cast<GLfloat>(x) / 100.f,
                    tf_block_vertices[f*18+i*3+1] + static_cast<GLfloat>(y) / 100.f,
                    tf_block_vertices[f*18+i*3+2] + static_cast<GLfloat>(z) / 100.f,
                    face_uv[i*2+0],
                    face_uv[i*2+1],
                    tex,
                    face_normal[BLOCK_FACE_TOP][0],
                    face_normal[BLOCK_FACE_TOP][1],
                    face_normal[BLOCK_FACE_TOP][2],
                });
            }
        }
    }
};

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

        ~Proxy() {
            arena->alloc_n_arena_blocks(arena_blocks, 0);
        }

        void upload_data(const vector<BlockVertexData>& vertices) {
            size_t n_ab = vertices.size() / ARENA_BLOCK_N_VERTICES;
            size_t rem = vertices.size() % ARENA_BLOCK_N_VERTICES;
            if (rem > 0) {
                n_ab += 1;
            } else {
                rem = ARENA_BLOCK_N_VERTICES;
            }

            arena->alloc_n_arena_blocks(arena_blocks, n_ab);

            for (size_t i = 0; i < n_ab-1; i++) {
                arena_blocks[i]->count = ARENA_BLOCK_N_VERTICES;
                arena->upload_data(arena_blocks[i], &vertices[ARENA_BLOCK_N_VERTICES * i]);
            }
            arena_blocks[n_ab-1]->count = rem;
            arena->upload_data(arena_blocks[n_ab-1], &vertices[ARENA_BLOCK_N_VERTICES * (n_ab-1)]);
        }
    };

private:
    GLuint         buffer;
    size_t         length = 0;
    vector<Block*> unused = {};
    vector<Block*> used   = {};

public:
    BlockVerticesArena() {
        buffer = gen_buffer();

        for (size_t i = 0; i < ARENA_INIT; i++) {
            unused.push_back(new Block(ARENA_BLOCK_SIZE * length));
            length += 1;
        }

        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, ARENA_BLOCK_SIZE * length, nullptr, GL_DYNAMIC_DRAW);

        RenderManager::instance().blocks_buffer = buffer;
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
    void expand() {
        GLuint new_buffer = gen_buffer();
        size_t new_length = length;

        for (size_t i = 0; i < ARENA_GROWTH; i++) {
            unused.push_back(new Block(ARENA_BLOCK_SIZE * new_length));
            new_length += 1;
        }

        glBindBuffer(GL_ARRAY_BUFFER, new_buffer);
        glBufferData(GL_ARRAY_BUFFER, ARENA_BLOCK_SIZE * new_length, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, buffer);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, ARENA_BLOCK_SIZE * length);

        glDeleteBuffers(1, &buffer);

        buffer = new_buffer;
        length = new_length;

        RenderManager::instance().blocks_buffer = buffer;
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
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
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
    class Chunk : private NonCopy<Chunk> {
    private:
        array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks = {};
        array<array<array<  bool, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> opaque = {};

        BlockVerticesArena::Proxy arena_proxy;

    public:
        Chunk(BlockVerticesArena* arena) : arena_proxy(BlockVerticesArena::Proxy(arena)) {}

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

            arena_proxy.upload_data(vertices);
        }

    private:
        Block*& _get_block(int32_t x, int32_t y, uint8_t z) {
            return blocks[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
        }

        bool& _get_opaque(int32_t x, int32_t y, uint8_t z) {
            return opaque[z][x & BLOCK_INDEX_MASK][y & BLOCK_INDEX_MASK];
        }
    };

public:
    BlockVerticesArena* arena;

private:
    unordered_map<uint64_t, Chunk> chunks      = {};
    unordered_set<uint64_t> chunks_need_update = {};

public:
    void add_block(Block* block) {
        uint64_t chunk_id = block_chunk_id(block->x, block->y);
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            chunk->second.add_block(block);
        } else {
            chunks.insert(make_pair(chunk_id, Chunk(arena)));
            chunks.at(chunk_id).add_block(block);
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

    const Chunk* get_chunk(uint64_t chunk_id) const {
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
                chunks.at(chunk_id).update_vertices(
                    get_chunk(chunk_id-(CHUNK_WIDTH<<32)),
                    get_chunk(chunk_id+(CHUNK_WIDTH<<32)),
                    get_chunk(chunk_id-(CHUNK_WIDTH    )),
                    get_chunk(chunk_id+(CHUNK_WIDTH    ))
                );
            }
            chunks_need_update.clear();
            arena->update_vertices();
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
