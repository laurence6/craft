#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <vector>

#include "config.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "util.hpp"

class UIElement : private NonCopy<UIElement> {
public:
    virtual ~UIElement() = default;
    virtual void render() const = 0;
};

class UICrosshair : public UIElement {
private:
    GLuint vao;

public:
    UICrosshair() {
        vector<GLfloat> data {
            -CROSSHAIR_X, 0.f, 0.f,
             CROSSHAIR_X, 0.f, 0.f,
            0.f, -CROSSHAIR_Y, 0.f,
            0.f,  CROSSHAIR_Y, 0.f,
        };
        GLuint vbo = gen_vbo();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);

        vao = gen_vao();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
        glBindVertexArray(0);
    }

    void render() const {
        ShaderManager::ins().line_shader.use();

        glLineWidth(CROSSHAIR_WIDTH);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
    }
};

class UITargetBlock : public UIElement {
private:
    GLuint vao;

public:
    UITargetBlock() {
        constexpr GLfloat _0 = -0.001f, _1 = 1.001f;
        vector<GLfloat> data {
            // bottom
            _0, _0, _0,
            _1, _0, _0,

            _1, _0, _0,
            _1, _1, _0,

            _1, _1, _0,
            _0, _1, _0,

            _0, _1, _0,
            _0, _0, _0,

            // top
            _0, _0, _1,
            _1, _0, _1,

            _1, _0, _1,
            _1, _1, _1,

            _1, _1, _1,
            _0, _1, _1,

            _0, _1, _1,
            _0, _0, _1,

            //
            _0, _0, _0,
            _0, _0, _1,

            _1, _0, _0,
            _1, _0, _1,

            _1, _1, _0,
            _1, _1, _1,

            _0, _1, _0,
            _0, _1, _1,
        };
        GLuint vbo = gen_vbo();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);

        vao = gen_vao();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
        glBindVertexArray(0);
    }

    void render() const {
        Block* target = Player::ins().target;
        if (!target) {
            return;
        }
        vec3 pos = vec3(static_cast<float>(target->x), static_cast<float>(target->y), static_cast<float>(target->z));
        mat4 mvp = Player::ins().get_mvp() * glm::translate(pos);

        ShaderManager::ins().block_edge_shader.use();
        ShaderManager::ins().block_edge_shader.upload_MVP(mvp);

        glLineWidth(BLOCK_EDGE_WIDTH);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);
    }
};

class UIManager : public Singleton<UIManager> {
private:
    vector<unique_ptr<UIElement>> ui_elements;

public:
    void init() {
        ui_elements.emplace_back(make_unique<UICrosshair>());
        ui_elements.emplace_back(make_unique<UITargetBlock>());
    }

    void shutdown() {
        ui_elements.clear();
    }

    void render() {
        for (auto const& e : ui_elements) {
            e->render();
        }
    }
};

#endif
