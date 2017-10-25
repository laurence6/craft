#include <cmath>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "block.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "util.hpp"

using namespace std;

const string VERTEX_SHADER_PATH = "shader/vertexshader.glsl";
const string FRAGMENT_SHADER_PATH = "shader/fragmentshader.glsl";
const string TEXTURE_FOLDER_PATH = "texture";
const string MAP_PATH = "map";

static GLuint load_shaders(string vertex_shader_path, string fragment_shader_path) {
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    string vertex_shader_code = "";
    ifstream vertex_shader_stream(vertex_shader_path);
    if (vertex_shader_stream.is_open()) {
        string line;
        while (getline(vertex_shader_stream, line))
            vertex_shader_code += line + "\n";
    } else {
        cerr << "Cannot open " << vertex_shader_path << endl;
        _exit(1);
    }

    string fragment_shader_code = "";
    ifstream fragment_shader_stream(fragment_shader_path);
    if (fragment_shader_stream.is_open()) {
        string line;
        while (getline(fragment_shader_stream, line))
            fragment_shader_code += line + "\n";
    }

    GLint result = GL_FALSE;
    int info_log_length;

    char const* vertex_shader_p = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_ID, 1, &vertex_shader_p, nullptr);
    glCompileShader(vertex_shader_ID);

    glGetShaderiv(vertex_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> vertex_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(vertex_shader_ID, info_log_length, nullptr, &vertex_shader_error_message[0]);
        log_vector(cerr, vertex_shader_error_message);
        _exit(1);
    }

    char const* fragment_shader_p = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_ID, 1, &fragment_shader_p, nullptr);
    glCompileShader(fragment_shader_ID);

    glGetShaderiv(fragment_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        vector<char> fragment_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(fragment_shader_ID, info_log_length, nullptr, &fragment_shader_error_message[0]);
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
        glGetProgramInfoLog(program_ID, info_log_length, nullptr, &program_error_message[0]);
        log_vector(cerr, program_error_message);
        _exit(1);
    }

    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}

static Scene load_map(string map_path) {
    ifstream map_file(map_path);
    if (!map_file.is_open()) {
        cerr << "Cannot open " << map_path << endl;
        _exit(1);
    }

    vector<Block> blocks;

    string t;
    int64_t x, y, z;
    while (true) {
        map_file >> t >> x >> y >> z;
        if (map_file.eof()) {
            break;
        }
        if (t == "dirt") {
            blocks.push_back(DirtBlock(x, y, z));
        } else if (t == "grass") {
            blocks.push_back(GrassBlock(x, y, z));
        } else if (t == "stone") {
            blocks.push_back(StoneBlock(x, y, z));
        }
    }

    return Scene(move(blocks));
}

static Camera camera = Camera();

static void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
            case GLFW_KEY_W: camera.start_move_forward(); break;
            case GLFW_KEY_S: camera.start_move_backward(); break;
            case GLFW_KEY_A: camera.start_move_left(); break;
            case GLFW_KEY_D: camera.start_move_right(); break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W: camera.stop_move_forward(); break;
            case GLFW_KEY_S: camera.stop_move_backward(); break;
            case GLFW_KEY_A: camera.stop_move_left(); break;
            case GLFW_KEY_D: camera.stop_move_right(); break;
        }
    }
}

static void cursor_pos_callback(GLFWwindow*, double posx, double posy) {
    static double last_posx = posx, last_posy = posy;

    float del_x = static_cast<float>(last_posx - posx);
    float del_y = static_cast<float>(posy - last_posy);
    last_posx = posx;
    last_posy = posy;

    camera.rotate(del_x, del_y);
}

int main() {
    Scene scene = load_map(MAP_PATH);
    vector<GLfloat> vertices = scene.get_vertices();
    vector<GLfloat> uv = scene.get_uv();
    auto texture_data = load_texture(TEXTURE_FOLDER_PATH);

    if (!glfwInit()) {
        _exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 960, "main", nullptr, nullptr);
    if (!window) {
        _exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        _exit(1);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint vertex_array_ID;
    glGenVertexArrays(1, &vertex_array_ID);
    glBindVertexArray(vertex_array_ID);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

    GLuint uv_buffer;
    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(GLfloat), &uv.front(), GL_STATIC_DRAW);

    GLuint program_ID = load_shaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    glUseProgram(program_ID);

    GLuint matrix_ID = glGetUniformLocation(program_ID, "MVP");

    GLuint texture_ID;
    glGenTextures(1, &texture_ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_ID);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, N_MIP_LEVEL, GL_RGB8, SUB_TEX_WIDTH, SUB_TEX_HEIGHT, N_TILES);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (uint8_t i = 0; i < N_MIP_LEVEL; i++) {
        uint32_t w = SUB_TEX_WIDTH >> i;
        uint32_t h = SUB_TEX_HEIGHT >> i;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, i, 0, 0, 0, w, h, N_TILES, GL_RGB, GL_UNSIGNED_BYTE, &texture_data[i].front());
    }
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    GLuint sampler_ID = glGetUniformLocation(program_ID, "sampler");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_ID);
    glUniform1i(sampler_ID, 0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &camera.get_mvp()[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);

        glfwPollEvents();

        camera.lerp_move();
    }

    _exit(0);
}