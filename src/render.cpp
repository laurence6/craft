#include <array>

#include <GL/glew.h>

#include "block.hpp"
#include "camera.hpp"
#include "render.hpp"
#include "texture.hpp"

void BlockShader::init() {
    Shader::init(SHADER_BLOCK_VERTEX_PATH, SHADER_BLOCK_FRAGMENT_PATH);

    MVP     = glGetUniformLocation(ID, "MVP");
    sun_dir = glGetUniformLocation(ID, "sun_dir");
    normals = glGetUniformLocation(ID, "normals[]");
    sampler = glGetUniformLocation(ID, "sampler");

    use();

    glUniform3fv(normals, 6, &face_normal[0][0]);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, N_MIP_LEVEL, GL_RGBA8, SUB_TEX_WIDTH, SUB_TEX_HEIGHT, N_TILES);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    const array<vector<uint8_t>, N_MIP_LEVEL> texture_data = load_texture(TEXTURE_FOLDER_PATH, 4);
    for (uint8_t i = 0; i < N_MIP_LEVEL; i++) {
        uint32_t w = SUB_TEX_WIDTH >> i;
        uint32_t h = SUB_TEX_HEIGHT >> i;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, i, 0, 0, 0, w, h, N_TILES, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[i].front());
    }
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glUniform1i(sampler, 0);
}

void RenderManager::render() const {
    block_shader.upload_MVP(Camera::instance().get_mvp());

    render_blocks();

    render_objects();

    render_ui();
}

void RenderManager::render_blocks() const {
    block_shader.use();

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, blocks_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertexData), (GLvoid *)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, blocks_buffer);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(BlockVertexData), (GLvoid *)(3 * sizeof(GLfloat)));

    glMultiDrawArrays(GL_TRIANGLES, &blocks_first[0], &blocks_count[0], blocks_first.size());

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void RenderManager::render_objects() const {
    // FIXME
    return;

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, objects_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData), (GLvoid *)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, objects_buffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData), (GLvoid *)(3 * sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, objects_n_triangles);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
