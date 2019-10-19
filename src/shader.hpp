#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>

#include "config.hpp"
#include "opengl.hpp"
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
    void init(string const& vertex_shader_path, string const& fragment_shader_path) {
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

    void upload_MVP(mat4 const& mvp) const {
        use();
        glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
    }

    void upload_sun_dir(vec3 const& dir) const {
        use();
        glUniform3f(sun_dir, dir.x, dir.y, dir.z);
    }
};

class BlockEdgeShader : public Shader {
private:
    GLuint MVP;

public:
    void init() {
        Shader::init(SHADER_BLOCK_EDGE_VERTEX_PATH, SHADER_BLOCK_EDGE_FRAGMENT_PATH);
        MVP = glGetUniformLocation(ID, "MVP");
    }

    void upload_MVP(mat4 const& mvp) const {
        use();
        glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
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
    BlockEdgeShader block_edge_shader;
    LineShader line_shader;

public:
    void init() {
        block_shader.init();
        block_edge_shader.init();
        line_shader.init();
    }
};

#endif
