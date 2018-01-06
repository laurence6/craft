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

class Shader {
protected:
    GLuint ID;

public:
    void use() const {
        glUseProgram(ID);
    }

protected:
    void init(const string vertex_shader_path, const string fragment_shader_path) {
        ID = load_shader(vertex_shader_path, fragment_shader_path);
    };
};

class BlockShader : public Shader {
private:
    GLuint MVP;
    GLuint sun_dir;
    GLuint sampler;

public:
    void init() {
        Shader::init(SHADER_BLOCK_VERTEX_PATH, SHADER_BLOCK_FRAGMENT_PATH);

        MVP = glGetUniformLocation(ID, "MVP");
        sun_dir = glGetUniformLocation(ID, "sun_dir");
        sampler = glGetUniformLocation(ID, "sampler");

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

    void upload_MVP(const mat4 mvp) const {
        use();
        glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
    }

    void upload_sun_dir(const vec3 dir) const {
        use();
        glUniform3f(sun_dir, dir.x, dir.y, dir.z);
    }
};

class LineShader : public Shader {
public:
    void init() {
        Shader::init(SHADER_LINE_VERTEX_PATH, SHADER_LINE_FRAGMENT_PATH);
    }
};

class RenderManager {
public:
    BlockShader block_shader;

    GLuint          blocks_buffer;
    vector<GLint>   blocks_first = {};
    vector<GLsizei> blocks_count = {};

private:
    LineShader  line_shader;

    GLuint objects_buffer;
    GLenum objects_n_triangles = 0;

    GLuint ui_buffer;

public:
    static RenderManager& instance() {
        static RenderManager ins;
        return ins;
    }

    void init() {
        block_shader.init();

        line_shader.init();

        objects_buffer = gen_buffer();

        ui_buffer = gen_buffer();
        upload_data(ui_buffer, vector<GLfloat> {
            -CROSSHAIR_X, 0.0, CROSSHAIR_X, 0.0,
            0.0, -CROSSHAIR_Y, 0.0, CROSSHAIR_Y,
        });
        glLineWidth(CROSSHAIR_WIDTH);
    }

    void upload_objects_data(const vector<GLfloat>& vertices, GLuint n_triangles) {
        upload_data(objects_buffer, vertices);
        objects_n_triangles = n_triangles;
    }

    void render() const;

private:
    RenderManager() = default;

    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&)      = delete;

    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&)      = delete;

    static void upload_data(GLuint buffer, const vector<GLfloat>& data) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);
    }

    void render_blocks() const {
        block_shader.use();

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, blocks_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid *)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, blocks_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, blocks_buffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));

        glMultiDrawArrays(GL_TRIANGLES, &blocks_first[0], &blocks_count[0], blocks_first.size());

        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    void render_objects() const {
        // FIXME
        return;

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, objects_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, objects_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLES, 0, objects_n_triangles);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    void render_ui() const {
        line_shader.use();

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, ui_buffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

        glDrawArrays(GL_LINES, 0, 6);

        glDisableVertexAttribArray(0);
    }
};

#endif
