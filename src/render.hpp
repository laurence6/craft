#ifndef RENDER_HPP
#define RENDER_HPP

#include <array>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

using namespace std;

class RenderManager {
private:
    unordered_map<uint64_t, array<GLuint, 3>> buffers_chunks = {};
    array<GLuint, 3> buffer_objects;

public:
    static RenderManager& instance() {
        static RenderManager ins;
        return ins;
    }

   void init() {
        glGenBuffers(2, &buffer_objects[0]);
    }

    void add_chunk(const uint64_t chunk_id) {
        array<GLuint, 3> buffer;
        glGenBuffers(2, &buffer[0]);
        buffer[2] = 0;
        buffers_chunks.emplace(chunk_id, move(buffer));
    }

    void upload_data_chunk(const uint64_t chunk_id, const vector<GLfloat>& vertices, const vector<GLfloat>& uv) {
        array<GLuint, 3>& buffer = buffers_chunks.at(chunk_id);
        upload_data(buffer, vertices, uv);
    }

    void upload_data_objects(const vector<GLfloat>& vertices, const vector<GLfloat>& uv) {
        upload_data(buffer_objects, vertices, uv);
    }

    void render() const {
        for (const pair<uint64_t, array<GLuint, 3>>& buffer : buffers_chunks) {
            render_buffer(buffer.second);
        }
        render_buffer(buffer_objects);
    }

private:
    RenderManager() {}

    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&)      = delete;

    ~RenderManager() {}

    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&)      = delete;

    static void upload_data(array<GLuint, 3>& buffer, const vector<GLfloat>& vertices, const vector<GLfloat>& uv) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv.front(), GL_STATIC_DRAW);

        buffer[2] = vertices.size() / 3;
    }

    static void render_buffer(const array<GLuint, 3>& buffer) {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_TRIANGLES, 0, buffer[2]);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }
};

#endif
