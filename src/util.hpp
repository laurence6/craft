#ifndef UTIL_HPP
#define UTIL_HPP

#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>

using namespace std;

constexpr float gravity_acc = 0.015f / 1000.f;
constexpr float fall_speed = 0.3f / 1000.f;
constexpr float jump_speed = 0.3f / 1000.f;

inline void _exit(int code) {
    glfwTerminate();
    exit(code);
}

inline void log_vector(ostream& o, vector<char> message) {
    for (const auto c : message) {
        o << c;
    }
    o << endl;
}

inline uint64_t time_now() {
    using namespace chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

template<uint32_t n>
array<GLuint, n> gen_buffers() {
    array<GLuint, n> buffers;
    glGenBuffers(n, &buffers[0]);
    return buffers;
}

GLuint load_shaders(string vertex_shader_path, string fragment_shader_path);

#endif
