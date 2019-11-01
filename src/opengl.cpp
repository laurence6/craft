#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "opengl.hpp"

using namespace std;

GLuint load_shader(string const& vertex_shader_path, string const& fragment_shader_path) {
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    string vertex_shader_code;
    ifstream vertex_shader_stream(vertex_shader_path);
    if (vertex_shader_stream.is_open()) {
        string line;
        while (getline(vertex_shader_stream, line)) {
            vertex_shader_code += line + "\n";
        }
    } else {
        cerr << "Cannot open " << vertex_shader_path << endl;
        throw new exception();
    }

    string fragment_shader_code;
    ifstream fragment_shader_stream(fragment_shader_path);
    if (fragment_shader_stream.is_open()) {
        string line;
        while (getline(fragment_shader_stream, line)) {
            fragment_shader_code += line + "\n";
        }
    }

    GLint result = GL_FALSE;
    int info_log_length;

    char const* vertex_shader_p = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_ID, 1, &vertex_shader_p, nullptr);
    glCompileShader(vertex_shader_ID);

    glGetShaderiv(vertex_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> error_message(info_log_length);
        glGetShaderInfoLog(vertex_shader_ID, info_log_length, nullptr, error_message.data());
        throw new runtime_error(error_message.data());
    }

    char const* fragment_shader_p = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_ID, 1, &fragment_shader_p, nullptr);
    glCompileShader(fragment_shader_ID);

    glGetShaderiv(fragment_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> error_message(info_log_length);
        glGetShaderInfoLog(fragment_shader_ID, info_log_length, nullptr, error_message.data());
        cerr << error_message.data() << endl;
        throw new runtime_error(error_message.data());
    }

    GLuint program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);

    glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
    glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> error_message(info_log_length);
        glGetProgramInfoLog(program_ID, info_log_length, nullptr, error_message.data());
        throw new runtime_error(error_message.data());
    }

    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}
