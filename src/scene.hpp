#ifndef SCENE_HPP
#define SCENE_HPP

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "object.hpp"

using namespace std;

class Scene {
public:
    Scene(vector<shared_ptr<Block>>&& _blocks, vector<shared_ptr<Object>>&& _objects) : objects(_objects) {
        for (const auto& block : _blocks) {
            blocks[block->coord] = block;
        }
        for (const auto& object : objects) {
            insert_vertices_uv(object);
        }
    }

    void add_object(Object&& _obj) {
        shared_ptr<Object> obj = make_shared<Object>(_obj);
        add_object(obj);
    }

    void add_object(const shared_ptr<Object> obj) {
        insert_vertices_uv(obj);
        objects.push_back(obj);
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

        for (const shared_ptr<Object>& object : objects) {
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
    void insert_vertices_uv(const shared_ptr<Object>& obj) {
        if (obj->vertices != nullptr)
            vertices.insert(vertices.end(), obj->vertices->begin(), obj->vertices->end());
        if (obj->uv != nullptr)
            uv.insert(uv.end(), obj->uv->begin(), obj->uv->end());
    }

    unordered_map<BlockCoord, shared_ptr<Block>, BlockCoordHasher> blocks;
    vector<shared_ptr<Object>> objects;

    vector<GLfloat> vertices;
    vector<GLfloat> uv;
};

#endif
