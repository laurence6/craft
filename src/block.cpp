#include <array>

#include "block.hpp"
#include "config.hpp"

using namespace std;

struct BlockData {
    bool is_opaque;
    bool has_six_faces;
    array<uint32_t, 6> tex;
};

static array<BlockData, 5> block_data {{
    { },
    { true, true, grass_block_tex },
    { true, true, dirt_block_tex },
    { true, true, stone_block_tex },
    { false, false, {{ grass_tex }} },
}};

bool Block::is_opaque() const {
    return block_data[type].is_opaque;
}

bool Block::has_six_faces() const {
    return block_data[type].has_six_faces;
}

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

void Block::insert_face_vertices(vector<BlockVertexData>& vertices, uint8_t f) const {
    if (has_six_faces()) {
        for (int i = 0; i < 6; i++) {
            vertices.emplace_back(BlockVertexData(
                id_block_vertices[f][i][0] + static_cast<GLfloat>(x),
                id_block_vertices[f][i][1] + static_cast<GLfloat>(y),
                id_block_vertices[f][i][2] + static_cast<GLfloat>(z),
                f,
                uv_coord[i],
                block_data[type].tex[f]
            ));
        }
    } else {
        for (int f = 0; f < 2; f++) {
            for (int i = 0; i < 6; i++) {
                vertices.emplace_back(BlockVertexData(
                    tf_block_vertices[f*6+i][0] + static_cast<GLfloat>(x),
                    tf_block_vertices[f*6+i][1] + static_cast<GLfloat>(y),
                    tf_block_vertices[f*6+i][2] + static_cast<GLfloat>(z),
                    FACE_TOP,
                    uv_coord[i],
                    block_data[type].tex[0]
                ));
            }
        }
    }
}
