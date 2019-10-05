#ifndef UI_HPP
#define UI_HPP

#include <vector>

#include "config.hpp"
#include "shader.hpp"
#include "util.hpp"

class UIManager {
public:
    GLuint vao;

public:
    static UIManager& instance() {
        static UIManager ins;
        return ins;
    }

    void init() {
        vector<GLfloat> data {
            -CROSSHAIR_X, 0.0, CROSSHAIR_X, 0.0,
            0.0, -CROSSHAIR_Y, 0.0, CROSSHAIR_Y,
        };
        GLuint vbo = gen_vbo();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);

        glLineWidth(CROSSHAIR_WIDTH);

        vao = gen_vao();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (GLvoid*)0);
        glBindVertexArray(0);
    }

    void render() const {
        ShaderManager::instance().line_shader.use();

        glBindVertexArray(vao);

        glDrawArrays(GL_LINES, 0, 6);
    }

private:
    UIManager() = default;

    UIManager(const UIManager&) = delete;
    UIManager(UIManager&&)      = delete;

    UIManager& operator=(const UIManager&) = delete;
    UIManager& operator=(UIManager&&)      = delete;
};

#endif
