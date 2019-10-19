#ifndef UTIL_HPP
#define UTIL_HPP

#include <chrono>
#include <string>

#include "opengl.hpp"

template<typename T>
class NonCopy {
protected:
    constexpr NonCopy() = default;

    NonCopy(NonCopy&&) noexcept = default;

    NonCopy& operator=(NonCopy&&) noexcept = default;

    ~NonCopy() = default;

public:
    NonCopy(const NonCopy&) = delete;

    NonCopy& operator=(const NonCopy&) = delete;
};

template<typename T>
class Singleton {
public:
    static T& ins() {
        static T ins;
        return ins;
    }

protected:
    Singleton() = default;

    ~Singleton() = default;

public:
    Singleton(Singleton const&) = delete;
    Singleton(Singleton&&)      = delete;

    Singleton& operator=(Singleton const&) = delete;
    Singleton& operator=(Singleton&&)      = delete;
};

void _exit(int code);

inline uint64_t time_now_ms() noexcept {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline uint64_t time_now_s() noexcept {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

inline GLuint gen_vbo() {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    return vbo;
}

inline void del_vbo(GLuint& vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
}

inline GLuint gen_vao() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return vao;
}

inline void del_vao(GLuint& vao) {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
}

GLuint load_shader(std::string const& vertex_shader_path, std::string const& fragment_shader_path);

#endif
