#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <array>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

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
            id += static_cast<uint64_t>(chunk_id.x) << 32;
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
    ChunkVertices() {
        vao = gen_vao();
        vbo = gen_vbo();

        // update vao
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertexData), (GLvoid*)0);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(BlockVertexData), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    void upload_data(vector<BlockVertexData> const& data) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BlockVertexData) * data.size(), data.data(), GL_STATIC_DRAW);

        count = data.size();
    }

    void render() const {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, count);
        glBindVertexArray(0);
    }

    ~ChunkVertices() {
        del_vao(vao);
        del_vbo(vbo);
    }
};

class Chunk : private NonCopy<Chunk> {
public:
    const ChunkID chunk_id;

private:
    array<array<array<Block*, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> blocks = {};
    array<array<array<  bool, CHUNK_WIDTH>, CHUNK_WIDTH>, 256> opaque = {};

    array<vector<BlockVertexData>, 4> vd_outer = {}; // four sides of a chunk
    vector<BlockVertexData>           vd_inner = {};

    ChunkVertices chunk_vertices;

public:
    Chunk(ChunkID chunk_id) : chunk_id(chunk_id) {
    }

    static vector<uint32_t> unload(Chunk* chunk);

    void add_block(Block* block) {
        _get_block(block->x, block->y, block->z) = block;
        _get_opaque(block->x, block->y, block->z) = Block::is_opaque(block);
    }

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        return _get_block(x, y, z);
    }

    void update_vertices(uint8_t flags, array<Chunk const*, 4> adj_chunks);

    void render() const {
        chunk_vertices.render();
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
    static uint32_t marshal(Block const* block) {
        uint32_t v = 0;
        v += (static_cast<uint32_t>(block->x) & BLOCK_INDEX_MASK) << 28;
        v += (static_cast<uint32_t>(block->y) & BLOCK_INDEX_MASK) << 24;
        v += static_cast<uint32_t>(block->z) << 16;
        v += static_cast<uint32_t>(block->id()) << 6;
        return v;
    }

    static Block* unmarshal(ChunkID chunk_id, uint32_t block) {
        constexpr uint32_t
            X_MASK  = 0xf000'0000,
            Y_MASK  = 0x0f00'0000,
            Z_MASK  = 0x00ff'0000,
            ID_MASK = 0x0000'ffc0;

        int32_t x = static_cast<int32_t>(((block & X_MASK) >> 28) + chunk_id.x);
        int32_t y = static_cast<int32_t>(((block & Y_MASK) >> 24) + chunk_id.y);
        uint8_t z = static_cast<uint8_t>((block & Z_MASK) >> 16);
        uint16_t id = static_cast<uint16_t>((block & ID_MASK) >> 6);

        return new_block(id, x, y, z);
    }
};

class BlockManager : private NonCopy<BlockManager> {
private:
    unordered_map<ChunkID, Chunk*, ChunkID::Hasher>  chunks             = {};
    unordered_map<ChunkID, uint8_t, ChunkID::Hasher> chunks_need_update = {};

public:
    void init() {
    }

    void add_block(Block* block);

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        ChunkID chunk_id { x, y };
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second->get_block(x, y, z);
        } else {
            return nullptr;
        }
    }

    Chunk const* get_chunk(ChunkID chunk_id) const {
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second;
        } else {
            return nullptr;
        }
    }

    void add_chunk_need_update(ChunkID chunk_id, uint8_t flags) {
        auto chunk_u_iter = chunks_need_update.find(chunk_id);
        if (chunk_u_iter != chunks_need_update.end()) {
            chunk_u_iter->second |= flags;
        } else {
            chunks_need_update.insert(make_pair(chunk_id, flags));
        }
    }

    void update_vertices();

    void render() const {
        for (auto const& chunk : chunks) {
            chunk.second->render();
        }
    }

private:
    static uint8_t boarder_flags(Block const* block) {
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
