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
                    auto adj_block = [&](uint8_t f, uint16_t _x, uint16_t _y) { return adj_chunks[f] == nullptr ? nullptr : &adj_chunks[f]->blocks[_x][_y][z]; };
                    auto insert    = [&](uint8_t f, BlockData const* other) {
                        if (other == nullptr || (!(other->is_opaque() && other->has_six_faces()) && (block.is_opaque() || block.type != other->type)))
                            block.insert_face_vertices(vertices, block_id, f);
                    };
                    insert(FACE_LEFT, x > 0 ? &blocks[x - 1][y][z] : adj_block(FACE_LEFT, CHUNK_WIDTH - 1, y));
                    insert(FACE_RIGHT, x < CHUNK_WIDTH - 1 ? &blocks[x + 1][y][z] : adj_block(FACE_RIGHT, 0, y));
                    insert(FACE_FRONT, y > 0 ? &blocks[x][y - 1][z] : adj_block(FACE_FRONT, x, CHUNK_WIDTH - 1));
                    insert(FACE_BACK, y < CHUNK_WIDTH - 1 ? &blocks[x][y + 1][z] : adj_block(FACE_BACK, x, 0));
                    insert(FACE_BOTTOM, z > 0 ? &blocks[x][y][z - 1] : nullptr);
                    insert(FACE_TOP, z < 255 ? &blocks[x][y][z + 1] : nullptr);
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
