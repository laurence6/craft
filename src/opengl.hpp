#ifndef OPENGL_HPP
#define OPENGL_HPP

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <string>

inline GLuint gen_vbo()
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    return vbo;
}

inline void del_vbo(GLuint& vbo)
{
    glDeleteBuffers(1, &vbo);
    vbo = 0;
}

inline GLuint gen_vao()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return vao;
}

inline void del_vao(GLuint& vao)
{
    glDeleteVertexArrays(1, &vao);
    vao = 0;
}

GLuint load_shader(std::string const& vertex_shader_path, std::string const& fragment_shader_path);

#endif
