#include <array>

#include "block.hpp"
#include "config.hpp"

bool BlockData::is_opaque() const {
    return block_config[type].is_opaque;
}

bool BlockData::has_six_faces() const {
    return block_config[type].has_six_faces;
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

void BlockData::insert_face_vertices(vector<BlockVertex>& vertices, BlockID const& block_id, uint8_t f) const {
    if (has_six_faces()) {
        for (int i = 0; i < 6; i++) {
            vertices.emplace_back(BlockVertex(
                id_block_vertices[f][i][0] + static_cast<GLfloat>(block_id.x),
                id_block_vertices[f][i][1] + static_cast<GLfloat>(block_id.y),
                id_block_vertices[f][i][2] + static_cast<GLfloat>(block_id.z),
                f,
                uv_coord[i],
                block_config[type].tex[f]
            ));
        }
    } else {
        for (int f = 0; f < 2; f++) {
            for (int i = 0; i < 6; i++) {
                vertices.emplace_back(BlockVertex(
                    tf_block_vertices[f*6+i][0] + static_cast<GLfloat>(block_id.x),
                    tf_block_vertices[f*6+i][1] + static_cast<GLfloat>(block_id.y),
                    tf_block_vertices[f*6+i][2] + static_cast<GLfloat>(block_id.z),
                    FACE_TOP,
                    uv_coord[i],
                    block_config[type].tex[0]
                ));
            }
        }
    }
}
