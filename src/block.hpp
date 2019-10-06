#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <vector>

#include <GL/glew.h>

#include "config.hpp"
#include "util.hpp"

using namespace std;

constexpr GLfloat _min = 0.00f, _max = 0.01f;
constexpr array<array<array<GLfloat, 3>, 6>, 6> id_block_vertices = {{
    [FACE_LEFT] = {{
        {{ _min, _min, _max }},
        {{ _min, _max, _max }},
        {{ _min, _max, _min }},

        {{ _min, _max, _min }},
        {{ _min, _min, _min }},
        {{ _min, _min, _max }},
    }},
    [FACE_RIGHT] = {{
        {{ _max, _max, _max }},
        {{ _max, _min, _max }},
        {{ _max, _min, _min }},

        {{ _max, _min, _min }},
        {{ _max, _max, _min }},
        {{ _max, _max, _max }},
    }},
    [FACE_FRONT] = {{
        {{ _max, _min, _max }},
        {{ _min, _min, _max }},
        {{ _min, _min, _min }},

        {{ _min, _min, _min }},
        {{ _max, _min, _min }},
        {{ _max, _min, _max }},
    }},
    [FACE_BACK] = {{
        {{ _min, _max, _max }},
        {{ _max, _max, _max }},
        {{ _max, _max, _min }},

        {{ _max, _max, _min }},
        {{ _min, _max, _min }},
        {{ _min, _max, _max }},
    }},
    [FACE_BOTTOM] = {{
        {{ _max, _min, _min }},
        {{ _min, _min, _min }},
        {{ _min, _max, _min }},

        {{ _min, _max, _min }},
        {{ _max, _max, _min }},
        {{ _max, _min, _min }},
    }},
    [FACE_TOP] = {{
        {{ _min, _max, _max }},
        {{ _min, _min, _max }},
        {{ _max, _min, _max }},

        {{ _max, _min, _max }},
        {{ _max, _max, _max }},
        {{ _min, _max, _max }},
    }},
}};
constexpr array<array<GLfloat, 3>, 12> tf_block_vertices = {{
    {{ _max, _min, _max }},
    {{ _min, _max, _max }},
    {{ _min, _max, _min }},

    {{ _min, _max, _min }},
    {{ _max, _min, _min }},
    {{ _max, _min, _max }},

    {{ _max, _max, _max }},
    {{ _min, _min, _max }},
    {{ _min, _min, _min }},

    {{ _min, _min, _min }},
    {{ _max, _max, _min }},
    {{ _max, _max, _max }},
}};

constexpr array<uint8_t, 6> uv_coord = {{
    0b10,
    0b00,
    0b01,
    0b01,
    0b11,
    0b10,
}};

constexpr array<array<GLfloat, 3>, 6> face_normal = {{
    [FACE_LEFT  ] = {{-0.4f, 0.0f, 0.9f }},
    [FACE_RIGHT ] = {{ 0.4f, 0.0f, 0.9f }},
    [FACE_FRONT ] = {{ 0.0f, 0.0f, 1.0f }},
    [FACE_BACK  ] = {{ 0.0f, 0.0f, 1.0f }},
    [FACE_BOTTOM] = {{ 0.0f, 0.0f,-1.0f }},
    [FACE_TOP   ] = {{ 0.0f, 0.0f, 1.0f }},
}};

struct BlockVertexData {
    GLfloat x, y, z;
    GLuint param; // 3 bits: face index, 2 bits: uv_coord, 27 bits: tex index

    constexpr BlockVertexData(GLfloat x, GLfloat y, GLfloat z, uint32_t face, uint32_t uv_coord, uint32_t tex) : x(x), y(y), z(z), param(gen_param(face, uv_coord, tex)) {}

private:
    constexpr GLuint gen_param(uint32_t face, uint32_t uv_coord, uint32_t tex) {
        GLuint param = 0;
        param += face << 29u;
        param += uv_coord << 27u;
        param += tex;
        return param;
    }
};

class Block : private NonCopy<Block> {
public:
    const int32_t x;
    const int32_t y;
    const uint8_t z;

private:
    const bool six_faces;

public:
    virtual ~Block() = default;

    virtual uint16_t id() const = 0;

    static bool is_opaque(const Block* block) {
        return block != nullptr && block->is_opaque();
    }

    bool has_six_faces() const {
        return six_faces;
    }

    virtual void insert_face_vertices(vector<BlockVertexData>&, uint8_t) const {}
    virtual void insert_face_vertices(vector<BlockVertexData>&) const {}

protected:
    Block(int32_t x, int32_t y, uint8_t z, bool six_faces) : x(x), y(y), z(z), six_faces(six_faces) {}

private:
    virtual bool is_opaque() const = 0;
};

class OpaqueBlock : public Block {
private:
    const array<uint32_t, 6> tex;

protected:
    OpaqueBlock(int32_t x, int32_t y, int32_t z, array<uint32_t, 6> tex) : Block(x, y, z, true), tex(tex) {}

private:
    bool is_opaque() const override {
        return true;
    }

    void insert_face_vertices(vector<BlockVertexData>& vertices, uint8_t f) const override {
        for (uint8_t i = 0; i < 6; i++) {
            vertices.emplace_back(BlockVertexData(
                id_block_vertices[f][i][0] + static_cast<GLfloat>(x) / 100.f,
                id_block_vertices[f][i][1] + static_cast<GLfloat>(y) / 100.f,
                id_block_vertices[f][i][2] + static_cast<GLfloat>(z) / 100.f,
                f,
                uv_coord[i],
                tex[f]
            ));
        }
    }
};

class GrassBlock : public OpaqueBlock {
public:
    GrassBlock(int32_t x, int32_t y, int32_t z) : OpaqueBlock(x, y, z, grass_block_tex) {}

private:
    uint16_t id() const override;
};

class DirtBlock : public OpaqueBlock {
public:
    DirtBlock(int32_t x, int32_t y, int32_t z) : OpaqueBlock(x, y, z, dirt_block_tex) {}

private:
    uint16_t id() const override;
};

class StoneBlock : public OpaqueBlock {
public:
    StoneBlock(int32_t x, int32_t y, int32_t z) : OpaqueBlock(x, y, z, stone_block_tex) {}

private:
    uint16_t id() const override;
};

class Grass : public Block {
private:
    const uint32_t tex;

public:
    Grass(int32_t x, int32_t y, uint8_t z) : Block(x, y, z, false), tex(grass_tex) {}

private:
    bool is_opaque() const override {
        return false;
    }

    void insert_face_vertices(vector<BlockVertexData>& vertices) const override {
        for (uint8_t f = 0; f < 2; f++) {
            for (uint8_t i = 0; i < 6; i++) {
                vertices.emplace_back(BlockVertexData(
                    tf_block_vertices[f*6+i][0] + static_cast<GLfloat>(x) / 100.f,
                    tf_block_vertices[f*6+i][1] + static_cast<GLfloat>(y) / 100.f,
                    tf_block_vertices[f*6+i][2] + static_cast<GLfloat>(z) / 100.f,
                    FACE_TOP,
                    uv_coord[i],
                    tex
                ));
            }
        }
    }

    uint16_t id() const override;
};

Block* new_block(uint16_t id, int32_t x, int32_t y, int32_t z);

#endif
