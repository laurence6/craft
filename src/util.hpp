#ifndef UTIL_HPP
#define UTIL_HPP

#include <chrono>
#include <string>

#include <GL/glew.h>

template<class T>
class NonCopy {
protected:
    constexpr NonCopy() = default;

    NonCopy(NonCopy&&) = default;

    NonCopy& operator=(NonCopy&&) = default;

private:
    NonCopy(const NonCopy&) = delete;

    NonCopy& operator=(const NonCopy&) = delete;
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

inline GLuint gen_buffer() {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    return buffer;
}

GLuint load_shader(std::string vertex_shader_path, std::string fragment_shader_path);

#endif
