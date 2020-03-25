#include "shader.hpp"

#include <array>

#include "block.hpp"
#include "texture.hpp"

constexpr array<array<GLfloat, 3>, 6> face_normal = { {
    [FACE_LEFT]   = { { -0.4f, 0.0f, 0.9f } },
    [FACE_RIGHT]  = { { 0.4f, 0.0f, 0.9f } },
    [FACE_FRONT]  = { { 0.0f, 0.0f, 1.0f } },
    [FACE_BACK]   = { { 0.0f, 0.0f, 1.0f } },
    [FACE_BOTTOM] = { { 0.0f, 0.0f, -1.0f } },
    [FACE_TOP]    = { { 0.0f, 0.0f, 1.0f } },
} };

void BlockShader::init()
{
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
    auto texture_data = load_texture(TEXTURE_FOLDER_PATH, 4);
    for (uint32_t i = 0; i < (uint32_t) N_MIP_LEVEL; i++)
    {
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
