#ifndef SCENE_HPP
#define SCENE_HPP

#include <chrono>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "object.hpp"

using namespace std;

class Scene {
public:
    static Scene& instance() {
        static Scene ins;
        return ins;
    }

    void add_block(Block* block) {
        blocks.add_block(block);
    }

    void add_object(Object* obj) {
        objects.push_back(obj);
    }

    void update_vertices_uv() {
        vertices.clear();
        uv.clear();
        for (const Block* block : blocks.get_blocks()) {
            insert_vertices_uv(block);
        }
        for (Object* obj : objects) {
            insert_vertices_uv(obj);
        }
    }

    const vector<GLfloat>& get_vertices() {
        return vertices;
    }

    const vector<GLfloat>& get_uv() {
        return uv;
    }

    void move_objects() {
        using namespace chrono;

        static auto last_update = system_clock::now();

        auto now = system_clock::now();
        auto del_t = duration_cast<milliseconds>(now - last_update);

        if (del_t < 16ms) {
            return;
        }

        float dt = static_cast<float>(del_t.count());

        for (Object* object : objects) {
            if (object->is_static) {
                continue;
            }

            vec3 del_p = object->velocity * dt;

            float len = length(del_p);
            if (len > 0.f) {
                object->pos += del_p;
            }
            // TODO: move vertices
        }

        last_update = now;
    }

private:
    Scene() {}

    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;

    ~Scene() {}

    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&)      = delete;

    void insert_vertices_uv(const Block* block) {
        vertices.insert(vertices.end(), block->get_vertices().begin(), block->get_vertices().end());
        uv.insert(uv.end(), block->get_uv().begin(), block->get_uv().end());
    }

    void insert_vertices_uv(Object* obj) {
        const vector<GLfloat>* _vertices = obj->get_vertices();
        if (_vertices != nullptr) {
            vertices.insert(vertices.end(), _vertices->begin(), _vertices->end());
        }
        const vector<GLfloat>* _uv = obj->get_uv();
        if (_uv != nullptr) {
            uv.insert(uv.end(), _uv->begin(), _uv->end());
        }
    }

    BlockManager    blocks  = BlockManager();
    vector<Object*> objects = {};

    vector<GLfloat> vertices = {};
    vector<GLfloat> uv       = {};
};

#endif
