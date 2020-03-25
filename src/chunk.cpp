#include "chunk.hpp"

#include "shader.hpp"
#include "util.hpp"

ChunkVertices::ChunkVertices()
{
    vao = gen_vao();
    vbo = gen_vbo();

    // update vao
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (GLvoid*) 0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(BlockVertex), (GLvoid*) (3 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

ChunkVertices::~ChunkVertices()
{
    del_vao(vao);
    del_vbo(vbo);
}

void ChunkVertices::upload_data(vector<BlockVertex> const& data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BlockVertex) * data.size(), data.data(), GL_STATIC_DRAW);

    count = data.size();
}

void ChunkVertices::render() const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

void Chunk::update(array<Chunk const*, 4>&& adj_chunks)
{
    vector<BlockVertex> vertices {};

#define UPDATE_OUTER_SURFACE(S, XY, X, Y, INDEX_ADJ)                                                                                          \
    for (uint16_t z = 0; z < 256; z++)                                                                                                        \
    {                                                                                                                                         \
        for (uint16_t XY = 0; (XY) < CHUNK_WIDTH; (XY)++)                                                                                     \
        {                                                                                                                                     \
            auto& block = blocks[X][Y][z];                                                                                                    \
            if (!block.is_null() && block.has_six_faces() && (adj_chunks[FACE_##S] == nullptr || !adj_chunks[FACE_##S]->opaque INDEX_ADJ[z])) \
            {                                                                                                                                 \
                auto block_id = to_block_id(X, Y, z);                                                                                         \
                block.insert_face_vertices(vertices, block_id, FACE_##S);                                                                     \
            }                                                                                                                                 \
        }                                                                                                                                     \
    }

    UPDATE_OUTER_SURFACE(LEFT, y, 0, y, [CHUNK_WIDTH - 1][y])
    UPDATE_OUTER_SURFACE(RIGHT, y, CHUNK_WIDTH - 1, y, [0][y])
    UPDATE_OUTER_SURFACE(FRONT, x, x, 0, [x][CHUNK_WIDTH - 1])
    UPDATE_OUTER_SURFACE(BACK, x, x, CHUNK_WIDTH - 1, [x][0])

#undef UPDATE_OUTER_SURFACE

    for (uint16_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (uint16_t y = 0; y < CHUNK_WIDTH; y++)
        {
            for (uint16_t z = 0; z < 256; z++)
            {
                auto& block = blocks[x][y][z];
                if (block.is_null())
                {
                    continue;
                }

                auto block_id = to_block_id(x, y, z);
                if (block.has_six_faces())
                {
                    if (x > 0 && !opaque[x - 1][y][z])
                        block.insert_face_vertices(vertices, block_id, FACE_LEFT);
                    if (x < CHUNK_WIDTH - 1 && !opaque[x + 1][y][z])
                        block.insert_face_vertices(vertices, block_id, FACE_RIGHT);
                    if (y > 0 && !opaque[x][y - 1][z])
                        block.insert_face_vertices(vertices, block_id, FACE_FRONT);
                    if (y < CHUNK_WIDTH - 1 && !opaque[x][y + 1][z])
                        block.insert_face_vertices(vertices, block_id, FACE_BACK);
                    if (z == 0 || !opaque[x][y][z - 1])
                        block.insert_face_vertices(vertices, block_id, FACE_BOTTOM);
                    if (z == 255 || !opaque[x][y][z + 1])
                        block.insert_face_vertices(vertices, block_id, FACE_TOP);
                }
                else
                {
                    block.insert_face_vertices(vertices, block_id, 0);
                }
            }
        }
    }

    chunk_vertices.upload_data(vertices);
}
