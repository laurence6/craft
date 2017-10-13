#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace std::chrono;
using namespace glm;

static const GLfloat g_color_buffer_data[36*3] = {
    0.0, 0.9, 0.0,
    0.0, 0.9, 0.0,
    0.0, 0.9, 0.0,
    0.0, 0.9, 0.0,
    0.0, 0.9, 0.0,
    0.0, 0.9, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.9, 0.0, 0.0,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.9,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
};

static void _exit(int code) {
    glfwTerminate();
    exit(code);
}

static void log_vector(ostream& o, vector<char> message) {
    for (const auto c : message) {
        o << c;
    }
    o << endl;
}

static void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static GLuint load_shaders(string vertex_file_path, string fragment_file_path) {
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    string vertex_shader_code;
    ifstream vertex_shader_stream(vertex_file_path);
    if (vertex_shader_stream.is_open()) {
        string line = "";
        while (getline(vertex_shader_stream, line))
            vertex_shader_code += "\n" + line;
        vertex_shader_stream.close();
    } else {
        cerr << "Cannot open " << vertex_file_path << endl;
        _exit(1);
    }

    string fragment_shader_code;
    ifstream fragment_shader_stream(fragment_file_path);
    if (fragment_shader_stream.is_open()) {
        string line = "";
        while (getline(fragment_shader_stream, line))
            fragment_shader_code += "\n" + line;
        fragment_shader_stream.close();
    }

    GLint result = GL_FALSE;
    int info_log_length;

    cout << "Compiling shader: " << vertex_file_path << endl;
    char const* vertex_shader_p = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_ID, 1, &vertex_shader_p, NULL);
    glCompileShader(vertex_shader_ID);

    glGetShaderiv(vertex_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> vertex_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(vertex_shader_ID, info_log_length, NULL, &vertex_shader_error_message[0]);
        log_vector(cerr, vertex_shader_error_message);
        _exit(1);
    }

    cout << "Compiling shader: " << fragment_file_path << endl;
    char const* fragment_shader_p = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_ID, 1, &fragment_shader_p, NULL);
    glCompileShader(fragment_shader_ID);

    glGetShaderiv(fragment_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> fragment_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(fragment_shader_ID, info_log_length, NULL, &fragment_shader_error_message[0]);
        log_vector(cerr, fragment_shader_error_message);
        _exit(1);
    }


    GLuint program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);


    glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
    glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> program_error_message(info_log_length + 1);
        glGetProgramInfoLog(program_ID, info_log_length, NULL, &program_error_message[0]);
        log_vector(cerr, program_error_message);
        _exit(1);
    }

    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}

static vector<GLfloat> load_map(string map_path) {
    vector<GLfloat> map_vertices;
    ifstream map_file(map_path);
    if (map_file.is_open()) {
        while (1) {
            double v;
            map_file >> v;
            if (map_file.eof()) {
                break;
            }
            map_vertices.push_back(v);
        }
    } else {
        cerr << "Cannot open " << map_path << endl;
        _exit(1);
    }
    if (map_vertices.size() % 3 != 0) {
        cerr << "Incorrect number of vertices " << map_vertices.size() << endl;
        _exit(1);
    }
    return map_vertices;
}

int main() {
    vector<GLfloat> vertices = load_map("map.vertices");
    vector<GLfloat> vertex_colors;
    for (unsigned int i = 0; i < vertices.size() / 3 / 36; i++) {
        for (unsigned int j = 0; j < 108; j++) {
            vertex_colors.push_back(g_color_buffer_data[j]);
        }
    }

    if (!glfwInit()) {
        _exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "main", NULL, NULL);
    if (!window) {
        _exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        _exit(1);
    }

    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint vertex_array_ID;
    glGenVertexArrays(1, &vertex_array_ID);
    glBindVertexArray(vertex_array_ID);

    GLuint program_ID = load_shaders("main.vertexshader", "main.fragmentshader");

    GLuint matrix_ID = glGetUniformLocation(program_ID, "MVP");
    mat4 projection  = perspective(radians(45.), 4. / 3., 0.1, 100.);
    mat4 view        = lookAt(vec3(0.5, 0.5, 0.2), vec3(0, 0, 0), vec3(0, 0, 1));
    mat4 mvp         = projection * view;

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

    GLuint color_buffer;
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_colors.size() * sizeof(GLfloat), &vertex_colors.front(), GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program_ID);

        long int t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        float angle = radians((float)(t / 10 % 400) * 0.9f);
        mat4 rotatex = rotate(angle, vec3(0, 0, 1));
        mat4 mat = mvp * rotatex;
        glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &mat[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    _exit(0);
}
