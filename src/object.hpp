#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include <GL/glew.h>

#include "math.hpp"

using namespace std;

class Object {
public:
    vector<GLfloat> vertices;
    vector<GLfloat> uv;

    bool is_static = true;

    vec3 pos = vec3(0.f, 0.f, 0.f);
    float rot = 0.f; // rotation around z axis / yaw
    vec3 velocity = vec3(0.f, 0.f, 0.f);
};

#endif
