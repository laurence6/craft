#include "chunk.hpp"
#include "shader.hpp"
#include "util.hpp"

ChunkVertices::ChunkVertices() {
    vao = gen_vao();
    vbo = gen_vbo();

    // update vao
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertexData), (GLvoid*)0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(BlockVertexData), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

void ChunkVertices::upload_data(vector<BlockVertexData> const& data) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BlockVertexData) * data.size(), data.data(), GL_STATIC_DRAW);

    count = data.size();
}

void ChunkVertices::render() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

ChunkVertices::~ChunkVertices() {
    del_vao(vao);
    del_vbo(vbo);
}

vector<uint32_t> Chunk::unload(Chunk* chunk) {
    vector<uint32_t> chunk_data {};

    for (uint16_t z = 0; z < 256; z++) {
        for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
            for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                Block* block = chunk->blocks[z][x][y];
                if (block != nullptr) {
                    chunk_data.push_back(marshal(block));
                    delete block;
                }
            }
        }
    }

    delete chunk;

    return chunk_data;
}

void Chunk::update_vertices(uint8_t flags, array<const Chunk*, 4>&& adj_chunks) {
#define UPDATE_OUTER_SURFACE(S, XY, INDEX_SELF, INDEX_ADJ)                                                                                           \
    if ((flags & FACE_##S##_BIT) != 0) {                                                                                                             \
        vd_outer[FACE_##S].clear();                                                                                                                  \
        for (uint16_t z = 0; z < 256; z++) {                                                                                                         \
            for (uint16_t XY = 0; XY < CHUNK_WIDTH; XY++) {                                                                                          \
                Block* block = blocks[z] INDEX_SELF;                                                                                                 \
                if (block != nullptr && block->has_six_faces() && (adj_chunks[FACE_##S] == nullptr || !adj_chunks[FACE_##S]->opaque[z] INDEX_ADJ)) { \
                    block->insert_face_vertices(vd_outer[FACE_##S], FACE_##S);                                                                       \
                }                                                                                                                                    \
            }                                                                                                                                        \
        }                                                                                                                                            \
    }

    UPDATE_OUTER_SURFACE(LEFT,  y, [0][y], [CHUNK_WIDTH-1][y])
    UPDATE_OUTER_SURFACE(RIGHT, y, [CHUNK_WIDTH-1][y], [0][y])
    UPDATE_OUTER_SURFACE(FRONT, x, [x][0], [x][CHUNK_WIDTH-1])
    UPDATE_OUTER_SURFACE(BACK,  x, [x][CHUNK_WIDTH-1], [x][0])

#undef UPDATE_OUTER_SURFACE

    vd_inner.clear();
    for (uint16_t z = 0; z < 256; z++) {
        for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
            for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
                Block* block = blocks[z][x][y];
                if (block == nullptr) continue;

                if (block->has_six_faces()) {
                    if (x > 0             && !opaque[z][x-1][y]) block->insert_face_vertices(vd_inner, FACE_LEFT );
                    if (x < CHUNK_WIDTH-1 && !opaque[z][x+1][y]) block->insert_face_vertices(vd_inner, FACE_RIGHT);
                    if (y > 0             && !opaque[z][x][y-1]) block->insert_face_vertices(vd_inner, FACE_FRONT);
                    if (y < CHUNK_WIDTH-1 && !opaque[z][x][y+1]) block->insert_face_vertices(vd_inner, FACE_BACK );

                    if (z == 0            || !opaque[z-1][x][y]) block->insert_face_vertices(vd_inner, FACE_BOTTOM);
                    if (z == 255          || !opaque[z+1][x][y]) block->insert_face_vertices(vd_inner, FACE_TOP   );
                } else {
                    block->insert_face_vertices(vd_inner);
                }
            }
        }
    }

    size_t c = vd_inner.size();
    for (int i = 0; i < 4; i++) {
        c += vd_outer[i].size();
    }

    vector<BlockVertexData> vertices {};
    vertices.reserve(c);

    vertices.insert(vertices.end(), vd_inner.begin(), vd_inner.end());
    for (int i = 0; i < 4; i++) {
        vertices.insert(vertices.end(), vd_outer[i].begin(), vd_outer[i].end());
    }

    chunk_vertices.upload_data(vertices);
}
