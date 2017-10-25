#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>

#include <GL/glew.h>

using namespace std;

using VerticesArray = array<GLfloat, 36 * 3>;
using UVArray = array<GLfloat, 36 * 3>;

inline VerticesArray calculate_block_vertices(int64_t x, int64_t y, int64_t z) {
    VerticesArray id_block = {
        1, 0, 1, // up
        0, 0, 1,
        0, 1, 1,

        1, 0, 1, // up
        1, 1, 1,
        0, 1, 1,

        1, 0, 1, // front
        1, 0, 0,
        0, 0, 0,

        1, 0, 1, // front
        0, 0, 1,
        0, 0, 0,

        0, 0, 1, // left
        0, 0, 0,
        0, 1, 0,

        0, 0, 1, // left
        0, 1, 1,
        0, 1, 0,

        0, 1, 1, // back
        0, 1, 0,
        1, 1, 0,

        0, 1, 1, // back
        1, 1, 1,
        1, 1, 0,

        1, 1, 1, // right
        1, 1, 0,
        1, 0, 0,

        1, 1, 1, // right
        1, 0, 1,
        1, 0, 0,

        1, 0, 0, // bottom
        0, 0, 0,
        0, 1, 0,

        1, 0, 0, // bottom
        1, 1, 0,
        0, 1, 0,
    };
    for (uintptr_t i = 0; i < 36*3; i += 3) {
        id_block[i+0] = (id_block[i+0] + static_cast<GLfloat>(x)) / 100.f;
        id_block[i+1] = (id_block[i+1] + static_cast<GLfloat>(y)) / 100.f;
        id_block[i+2] = (id_block[i+2] + static_cast<GLfloat>(z)) / 100.f;
    }
    return id_block;
}

class Block {
public:
    const VerticesArray vertices;
    const UVArray& uv;

protected:
    Block(int64_t x, int64_t y, int64_t z, const UVArray& uv) : vertices(calculate_block_vertices(x, y, z)), uv(uv) {}
};

constexpr UVArray dirt_block_uv = {
    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,
};
class DirtBlock : public Block {
public:
    DirtBlock(int64_t x, int64_t y, int64_t z) : Block(x, y, z, dirt_block_uv) {}
};

constexpr UVArray grass_block_uv = {
    1.0, 0.0, 0,
    0.0, 0.0, 0,
    0.0, 1.0, 0,

    1.0, 0.0, 0,
    1.0, 1.0, 0,
    0.0, 1.0, 0,

    1.0, 0.0, 1,
    1.0, 1.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    0.0, 0.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    1.0, 1.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    0.0, 0.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    1.0, 1.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    0.0, 0.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    1.0, 1.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 1,
    0.0, 0.0, 1,
    0.0, 1.0, 1,

    1.0, 0.0, 2,
    0.0, 0.0, 2,
    0.0, 1.0, 2,

    1.0, 0.0, 2,
    1.0, 1.0, 2,
    0.0, 1.0, 2,
};
class GrassBlock : public Block {
public:
    GrassBlock(int64_t x, int64_t y, int64_t z) : Block(x, y, z, grass_block_uv) {}
};

constexpr UVArray stone_block_uv = {
    1.0, 0.0, 3,
    0.0, 0.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    1.0, 1.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    1.0, 1.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    0.0, 0.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    1.0, 1.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    0.0, 0.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    1.0, 1.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    0.0, 0.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    1.0, 1.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    0.0, 0.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    0.0, 0.0, 3,
    0.0, 1.0, 3,

    1.0, 0.0, 3,
    1.0, 1.0, 3,
    0.0, 1.0, 3,
};
class StoneBlock : public Block {
public:
    StoneBlock(int64_t x, int64_t y, int64_t z) : Block(x, y, z, stone_block_uv) {}
};

#endif
