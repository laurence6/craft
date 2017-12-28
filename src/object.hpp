#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include <GL/glew.h>

#include "collider.hpp"
#include "math.hpp"
#include "render.hpp"

using namespace std;

enum Status {
    Fixed,
    Normal,
    Jumping,
    Falling,
};

class Object {
public:
    Status status = Status::Normal;

    vec3 pos;
    float rot = 0.f; // rotation around z axis / yaw
    vec3 velocity = vec3(0.f, 0.f, 0.f);

    Collider* collider = nullptr;

public:
    void jump() {
        if (status == Status::Normal) {
            status = Status::Jumping;
            velocity.z = 0.005f / 1000.f * 16.f;
        }
    }

    virtual const vector<GLfloat>* get_vertices() const {
        return nullptr;
    }

    virtual const vector<GLfloat>* get_uv() const {
        return nullptr;
    }
};

class ObjectManager {
private:
    vector<Object*> objects = {};

    vector<GLfloat> vertices = {};
    vector<GLfloat> uv       = {};

public:
    void add_object(Object* obj) {
        objects.push_back(obj);
    }

    void update_vertices_uv() {
        vertices.clear();
        uv.clear();
        for (const Object* obj : objects) {
            const vector<GLfloat>* _vertices = obj->get_vertices();
            if (_vertices != nullptr) {
                vertices.insert(vertices.end(), _vertices->begin(), _vertices->end());
            }
            const vector<GLfloat>* _uv = obj->get_uv();
            if (_uv != nullptr) {
                uv.insert(uv.end(), _uv->begin(), _uv->end());
            }
        }

        RenderManager::instance().upload_data_objects(vertices, uv, GL_TRIANGLES);
    }

    const vector<Object*>& get_objects() {
        return objects;
    }

    const vector<GLfloat>& get_vertices() const {
        return vertices;
    }

    const vector<GLfloat>& get_uv() const {
        return uv;
    }
};

#endif
