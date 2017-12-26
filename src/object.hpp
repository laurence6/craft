#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include <GL/glew.h>

#include "collider.hpp"
#include "math.hpp"

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

#endif
