#ifndef RENDER_HPP
#define RENDER_HPP

#include <array>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

#include "config.hpp"
#include "util.hpp"
#include "texture.hpp"

using namespace std;

class RenderElement {
public:
    static GLuint program_ID;
    static GLuint matrix_ID;
    static GLuint sampler_ID;

    const GLuint vertices_buffer;
    GLuint mode = 0;
    GLuint n_triangles = 0;

public:
    RenderElement(GLuint _vertices_buffer) : vertices_buffer(_vertices_buffer) {}
};

class Camera;

class RenderManager {
private:
    unordered_map<uint64_t, RenderElement> chunks = {};
    RenderElement* objects = nullptr;

public:
    static RenderManager& instance() {
        static RenderManager ins;
        return ins;
    }

    void init() {
        RenderElement::program_ID = load_shaders(SHADER_BLOCK_VERTEX_PATH, SHADER_BLOCK_FRAGMENT_PATH);
        RenderElement::matrix_ID = glGetUniformLocation(RenderElement::program_ID, "MVP");
        RenderElement::sampler_ID = glGetUniformLocation(RenderElement::program_ID, "sampler");

        GLuint texture_ID;
        glGenTextures(1, &texture_ID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_ID);
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
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_ID);
        glUniform1i(RenderElement::sampler_ID, 0);

        objects = new RenderElement(gen_buffer());
    }

    void add_chunk(const uint64_t chunk_id) {
        chunks.emplace(make_pair(chunk_id, RenderElement(gen_buffer())));
    }

    void upload_data_chunk(const uint64_t chunk_id, const vector<GLfloat>& vertices, GLuint mode, GLuint n_triangles) {
        RenderElement& chunk = chunks.at(chunk_id);
        upload_data(chunk, vertices, mode, n_triangles);
    }

    void upload_data_objects(const vector<GLfloat>& vertices, GLuint mode, GLuint n_triangles) {
        upload_data(*objects, vertices, mode, n_triangles);
    }

    void render() const;

private:
    RenderManager() {}

    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&)      = delete;

    ~RenderManager() {}

    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&)      = delete;

    static void upload_data(RenderElement& element, const vector<GLfloat>& vertices, GLuint mode, GLuint n_triangles) {
        glBindBuffer(GL_ARRAY_BUFFER, element.vertices_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

        element.n_triangles = n_triangles;
        element.mode = mode;
    }

    static void render_element(const RenderElement& element) {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, element.vertices_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, element.vertices_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

        glDrawArrays(element.mode, 0, element.n_triangles);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }
};

#endif
