#ifndef UTIL_HPP
#define UTIL_HPP

#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

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

inline uint64_t time_now() noexcept {
    using namespace chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline GLuint gen_buffer() noexcept {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    return buffer;
}

GLuint load_shaders(string vertex_shader_path, string fragment_shader_path);

#endif
