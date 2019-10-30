#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>

#include "opengl.hpp"
#include "util.hpp"

using namespace std;

struct BlockVertexData {
    GLfloat x, y, z;
    GLuint param; // 3 bits: face index, 2 bits: uv_coord, 27 bits: tex index

    constexpr BlockVertexData(GLfloat x, GLfloat y, GLfloat z, uint32_t face, uint32_t uv_coord, uint32_t tex) : x(x), y(y), z(z), param(gen_param(face, uv_coord, tex)) {
    }

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
    int32_t x;
    int32_t y;
    uint8_t z;
    uint16_t type;

public:
    Block() = default;

    Block(int32_t x, int32_t y, uint8_t z, uint16_t type) : x(x), y(y), z(z), type(type) {
    }

    void clear() {
        type = 0;
    }

    bool is_null() const {
        return type == 0;
    }

    bool is_opaque() const;

    bool has_six_faces() const;

    void insert_face_vertices(vector<BlockVertexData>&, uint8_t) const;
};

#endif
