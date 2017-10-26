#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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

GLuint load_shaders(string vertex_shader_path, string fragment_shader_path);

#endif
