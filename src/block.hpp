#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>

#include "math.hpp"
#include "opengl.hpp"

using namespace std;

struct BlockVertex
{
    GLfloat x, y, z;
    GLuint  param; // 3 bits: face index, 2 bits: uv_coord, 27 bits: tex index

    BlockVertex(GLfloat x, GLfloat y, GLfloat z, uint32_t face, uint32_t uv_coord, uint32_t tex) : x(x), y(y), z(z)
    {
        param = 0;
        param |= face << 29u;
        param |= uv_coord << 27u;
        param |= tex;
    }
};

class BlockID
{
public:
    int32_t x;
    int32_t y;
    uint8_t z;

public:
    template<typename T1, typename T2, typename T3>
    BlockID(T1 x, T2 y, T3 z) : x(x), y(y), z(z)
    {
    }

    [[nodiscard]] vec3 to_vec3() const
    {
        return vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
    }
};

class BlockData
{
public:
    uint16_t type = 0;

public:
    void clear()
    {
        type = 0;
    }

    [[nodiscard]] bool is_null() const
    {
        return type == 0;
    }

    [[nodiscard]] bool is_opaque() const;

    [[nodiscard]] bool has_six_faces() const;

    void insert_face_vertices(vector<BlockVertex>& vertices, BlockID const& block_id, uint8_t f) const;
};

#endif
