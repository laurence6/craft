#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>

#include "config.hpp"
#include "util.hpp"

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
    GLuint normals;
    GLuint sampler;

public:
    void init();

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

    void render_blocks() const;

    void render_objects() const;

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
