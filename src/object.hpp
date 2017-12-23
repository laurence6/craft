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
    virtual const vector<GLfloat>* get_vertices() {
        return nullptr;
    }

    virtual const vector<GLfloat>* get_uv() {
        return nullptr;
    }

    Status status = Status::Normal;

    vec3 pos;
    float rot = 0.f; // rotation around z axis / yaw
    vec3 velocity = vec3(0.f, 0.f, 0.f);

    Collider* collider = nullptr;
};

#endif
