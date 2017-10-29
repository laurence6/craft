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

// 0-------1-------2-------3-------4-------5-------6-------7-------
// |8 z    |28 x mod 0xfffffff         |28 y mod 0xfffffff
inline int64_t hash_block_coord(int64_t x, int64_t y, uint8_t z) noexcept {
    int64_t h = 0;
    h += static_cast<uint64_t>(z) << 56;
    h += (x & 0xfff'ffff) << 28;
    h += (y & 0xfff'ffff) <<  0;
    return h;
}

class Block {
public:
    inline int64_t hash_coord() const noexcept {
        return hash_block_coord(x, y, z);
    }

    inline void update_vertices_uv(const unordered_map<int64_t, Block*>& blocks) {
        vertices.clear();
        uv.clear();
        if (blocks.count(hash_block_coord(x-1, y, z)) == 0) insert_face_vertices_uv(2);
        if (blocks.count(hash_block_coord(x+1, y, z)) == 0) insert_face_vertices_uv(4);
        if (blocks.count(hash_block_coord(x, y-1, z)) == 0) insert_face_vertices_uv(1);
        if (blocks.count(hash_block_coord(x, y+1, z)) == 0) insert_face_vertices_uv(3);
        if (z == numeric_limits<uint8_t>::max() ||
                blocks.count(hash_block_coord(x, y, z+1)) == 0) insert_face_vertices_uv(0);
        if (z == numeric_limits<uint8_t>::min() ||
                blocks.count(hash_block_coord(x, y, z-1)) == 0) insert_face_vertices_uv(5);
    }

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

    const int64_t x;
    const int64_t y;
    const uint8_t z;
    const array<GLfloat, 6> tex;

    vector<GLfloat> vertices;
    vector<GLfloat> uv;

protected:
    Block(int64_t x, int64_t y, uint8_t z, array<GLfloat, 6> tex) : x(x), y(y), z(z), tex(tex) {}
};

static const array<GLfloat, 6> dirt_block_tex = { 2, 2, 2, 2, 2, 2 };
class DirtBlock : public Block {
public:
    DirtBlock(int64_t x, int64_t y, uint8_t z) : Block(x, y, z, dirt_block_tex) {}
};

static const array<GLfloat, 6> grass_block_tex = { 0, 1, 1, 1, 1, 2 };
class GrassBlock : public Block {
public:
    GrassBlock(int64_t x, int64_t y, uint8_t z) : Block(x, y, z, grass_block_tex) {}
};

static const array<GLfloat, 6> stone_block_tex = { 3, 3, 3, 3, 3, 3 };
class StoneBlock : public Block {
public:
    StoneBlock(int64_t x, int64_t y, uint8_t z) : Block(x, y, z, stone_block_tex) {}
};

#endif
