#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>

#include "opengl.hpp"
#include "util.hpp"

using namespace std;

struct BlockVertexData {
    GLfloat x, y, z;
    GLuint param; // 3 bits: face index, 2 bits: uv_coord, 27 bits: tex index

    BlockVertexData(GLfloat x, GLfloat y, GLfloat z, uint32_t face, uint32_t uv_coord, uint32_t tex) : x(x), y(y), z(z) {
        param = 0;
        param |= face << 29u;
        param |= uv_coord << 27u;
        param |= tex;
    }
};

class BlockID {
public:
    int32_t x;
    int32_t y;
    uint8_t z;

public:
    template<typename T1, typename T2, typename T3>
    BlockID(T1 x, T2 y, T3 z) : x(x), y(y), z(z) {
    }
};

class BlockData : private NonCopy<BlockData> {
public:
    uint16_t type = 0;

public:
    BlockData() = default;

    BlockData(uint16_t type) : type(type) {
    }

    void clear() {
        type = 0;
    }

    bool is_null() const {
        return type == 0;
    }

    bool is_opaque() const;

    bool has_six_faces() const;

    void insert_face_vertices(vector<BlockVertexData>& vertices, BlockID const& block_id, uint8_t f) const;
};

#endif
