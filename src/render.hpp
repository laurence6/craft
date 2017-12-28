#ifndef RENDER_HPP
#define RENDER_HPP

#include <array>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

#include "util.hpp"

using namespace std;

class RenderElement {
public:
    const GLuint buffer_vertices;
    const GLuint buffer_uv;
    GLuint mode = 0;
    GLuint n_triangles = 0;

public:
    RenderElement(GLuint _buffer_vertices, GLuint _buffer_uv) : buffer_vertices(_buffer_vertices), buffer_uv(_buffer_uv) {}
};

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
        array<GLuint, 2> buffers = gen_buffers<2>();
        objects = new RenderElement(buffers[0], buffers[1]);
    }

    void add_chunk(const uint64_t chunk_id) {
        array<GLuint, 2> buffers = gen_buffers<2>();
        chunks.emplace(make_pair(chunk_id, RenderElement(buffers[0], buffers[1])));
    }

    void upload_data_chunk(const uint64_t chunk_id, const vector<GLfloat>& vertices, const vector<GLfloat>& uv, GLuint mode) {
        RenderElement& chunk = chunks.at(chunk_id);
        upload_data(chunk, vertices, uv, mode);
    }

    void upload_data_objects(const vector<GLfloat>& vertices, const vector<GLfloat>& uv, GLuint mode) {
        upload_data(*objects, vertices, uv, mode);
    }

    void render() const {
        for (const pair<uint64_t, RenderElement>& chunk : chunks) {
            render_element(chunk.second);
        }
        render_element(*objects);
    }

private:
    RenderManager() {}

    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&)      = delete;

    ~RenderManager() {}

    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&)      = delete;

    static void upload_data(RenderElement& element, const vector<GLfloat>& vertices, const vector<GLfloat>& uv, GLuint mode) {
        glBindBuffer(GL_ARRAY_BUFFER, element.buffer_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, element.buffer_uv);
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv.front(), GL_STATIC_DRAW);

        element.n_triangles = vertices.size() / 3;
        element.mode = mode;
    }

    static void render_element(const RenderElement& element) {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, element.buffer_vertices);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, element.buffer_uv);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(element.mode, 0, element.n_triangles);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }
};

#endif
