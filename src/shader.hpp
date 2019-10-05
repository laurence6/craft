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

class ShaderManager : public Singleton<ShaderManager> {
public:
    BlockShader block_shader;

    LineShader  line_shader;

public:
    void init() {
        block_shader.init();

        line_shader.init();
    }

private:
    static void upload_data(GLuint vbo, const vector<GLfloat>& data) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
    }
};

#endif
