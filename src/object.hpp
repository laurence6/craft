#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "math.hpp"

using namespace std;

class Object {
public:
    virtual const vector<GLfloat>* get_vertices() {
        return nullptr;
    }

    virtual const vector<GLfloat>* get_uv() {
        return nullptr;
    }

    bool is_static = true;

    vec3 pos;
    float rot = 0.f; // rotation around z axis / yaw
    vec3 velocity = vec3(0.f, 0.f, 0.f);
};

#endif
