#ifndef _UTIL_H
#define _UTIL_H

#include <cstdlib>
#include <iostream>
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

#endif
