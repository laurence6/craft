#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>

#include "opengl.hpp"
#include "util.hpp"

using namespace std;

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
    const bool opaque;
    const bool six_faces;

public:
    virtual ~Block() = default;

    virtual uint16_t id() const = 0;

    bool is_opaque() const {
        return opaque;
    }

    bool has_six_faces() const {
        return six_faces;
    }

    virtual void insert_face_vertices(vector<BlockVertexData>&, uint8_t) const {}
    virtual void insert_face_vertices(vector<BlockVertexData>&) const {}

protected:
    Block(int32_t x, int32_t y, uint8_t z, bool opaque, bool six_faces) : x(x), y(y), z(z), opaque(opaque), six_faces(six_faces) {
    }
};

Block* new_block(uint16_t id, int32_t x, int32_t y, uint8_t z);

#endif
