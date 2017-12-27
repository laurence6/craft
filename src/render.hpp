#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>

#include <GL/glew.h>

using namespace std;

class RenderManager {
private:
    GLuint vertex_buffer;
    GLuint uv_buffer;

public:
    static RenderManager& instance() {
        static RenderManager ins;
        return ins;
    }

    void init() {
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &uv_buffer);
    }

    void render(const vector<GLfloat>& vertices, const vector<GLfloat>& uv) {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv.front(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

private:
    RenderManager() {}

    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&)      = delete;

    ~RenderManager() {}

    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&)      = delete;
};

#endif
