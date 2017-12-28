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
    const GLuint vertices_buffer;
    GLuint mode = 0;
    GLuint n_triangles = 0;

public:
    RenderElement(GLuint _vertices_buffer) : vertices_buffer(_vertices_buffer) {}
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

    static void upload_data(RenderElement& element, const vector<GLfloat>& vertices, GLuint mode, GLuint n_triangles) {
        glBindBuffer(GL_ARRAY_BUFFER, element.vertices_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

        element.n_triangles = n_triangles;
        element.mode = mode;
    }

    static void render_element(const RenderElement& element) {
        // [ [[1,2,3],[4,5,6]], ... ]
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
