#ifndef BLOCK_DATA_HPP
#define BLOCK_DATA_HPP

#include <array>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "config.hpp"

using namespace std;

constexpr GLfloat _0 = 0.f, _1 = 1.f;
constexpr array<array<array<GLfloat, 3>, 6>, 6> id_block_vertices = {{
    [FACE_LEFT] = {{
        {{ _0, _0, _1 }},
        {{ _0, _1, _1 }},
        {{ _0, _1, _0 }},

        {{ _0, _1, _0 }},
        {{ _0, _0, _0 }},
        {{ _0, _0, _1 }},
    }},
    [FACE_RIGHT] = {{
        {{ _1, _1, _1 }},
        {{ _1, _0, _1 }},
        {{ _1, _0, _0 }},

        {{ _1, _0, _0 }},
        {{ _1, _1, _0 }},
        {{ _1, _1, _1 }},
    }},
    [FACE_FRONT] = {{
        {{ _1, _0, _1 }},
        {{ _0, _0, _1 }},
        {{ _0, _0, _0 }},

        {{ _0, _0, _0 }},
        {{ _1, _0, _0 }},
        {{ _1, _0, _1 }},
    }},
    [FACE_BACK] = {{
        {{ _0, _1, _1 }},
        {{ _1, _1, _1 }},
        {{ _1, _1, _0 }},

        {{ _1, _1, _0 }},
        {{ _0, _1, _0 }},
        {{ _0, _1, _1 }},
    }},
    [FACE_BOTTOM] = {{
        {{ _1, _0, _0 }},
        {{ _0, _0, _0 }},
        {{ _0, _1, _0 }},

        {{ _0, _1, _0 }},
        {{ _1, _1, _0 }},
        {{ _1, _0, _0 }},
    }},
    [FACE_TOP] = {{
        {{ _0, _1, _1 }},
        {{ _0, _0, _1 }},
        {{ _1, _0, _1 }},

        {{ _1, _0, _1 }},
        {{ _1, _1, _1 }},
        {{ _0, _1, _1 }},
    }},
}};
constexpr array<array<GLfloat, 3>, 12> tf_block_vertices = {{
    {{ _1, _0, _1 }},
    {{ _0, _1, _1 }},
    {{ _0, _1, _0 }},

    {{ _0, _1, _0 }},
    {{ _1, _0, _0 }},
    {{ _1, _0, _1 }},

    {{ _1, _1, _1 }},
    {{ _0, _0, _1 }},
    {{ _0, _0, _0 }},

    {{ _0, _0, _0 }},
    {{ _1, _1, _0 }},
    {{ _1, _1, _1 }},
}};

constexpr array<uint8_t, 6> uv_coord = {{
    0b10,
    0b00,
    0b01,
    0b01,
    0b11,
    0b10,
}};

class OpaqueBlock : public Block {
private:
    const array<uint32_t, 6> tex;

protected:
    OpaqueBlock(int32_t x, int32_t y, uint8_t z, array<uint32_t, 6> tex) : Block(x, y, z, true, true), tex(tex) {}

private:
    void insert_face_vertices(vector<BlockVertexData>& vertices, uint8_t f) const override {
        for (uint8_t i = 0; i < 6; i++) {
            vertices.emplace_back(BlockVertexData(
                id_block_vertices[f][i][0] + static_cast<GLfloat>(x),
                id_block_vertices[f][i][1] + static_cast<GLfloat>(y),
                id_block_vertices[f][i][2] + static_cast<GLfloat>(z),
                f,
                uv_coord[i],
                tex[f]
            ));
        }
    }
};

class GrassBlock : public OpaqueBlock {
public:
    GrassBlock(int32_t x, int32_t y, uint8_t z) : OpaqueBlock(x, y, z, grass_block_tex) {}

private:
    uint16_t id() const override;
};

class DirtBlock : public OpaqueBlock {
public:
    DirtBlock(int32_t x, int32_t y, uint8_t z) : OpaqueBlock(x, y, z, dirt_block_tex) {}

private:
    uint16_t id() const override;
};

class StoneBlock : public OpaqueBlock {
public:
    StoneBlock(int32_t x, int32_t y, uint8_t z) : OpaqueBlock(x, y, z, stone_block_tex) {}

private:
    uint16_t id() const override;
};

class Grass : public Block {
private:
    const uint32_t tex;

public:
    Grass(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, false, false), tex(grass_tex) {}

private:
    void insert_face_vertices(vector<BlockVertexData>& vertices) const override {
        for (uint8_t f = 0; f < 2; f++) {
            for (uint8_t i = 0; i < 6; i++) {
                vertices.emplace_back(BlockVertexData(
                    tf_block_vertices[f*6+i][0] + static_cast<GLfloat>(x),
                    tf_block_vertices[f*6+i][1] + static_cast<GLfloat>(y),
                    tf_block_vertices[f*6+i][2] + static_cast<GLfloat>(z),
                    FACE_TOP,
                    uv_coord[i],
                    tex
                ));
            }
        }
    }

    uint16_t id() const override;
};

#endif
