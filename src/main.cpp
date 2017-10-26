#include <cmath>
#include <fstream>
#include <memory>
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

static Scene load_map(string map_path) {
    ifstream map_file(map_path);
    if (!map_file.is_open()) {
        cerr << "Cannot open " << map_path << endl;
        _exit(1);
    }

    vector<shared_ptr<Object>> blocks;

    string t;
    int64_t x, y, z;
    while (true) {
        map_file >> t >> x >> y >> z;
        if (map_file.eof()) {
            break;
        }
        if (t == "dirt") {
            blocks.push_back(shared_ptr<Object>(new DirtBlock(x, y, z)));
        } else if (t == "grass") {
            blocks.push_back(shared_ptr<Object>(new GrassBlock(x, y, z)));
        } else if (t == "stone") {
            blocks.push_back(shared_ptr<Object>(new StoneBlock(x, y, z)));
        }
    }

    return Scene(move(blocks));
}

static shared_ptr<Camera> camera = shared_ptr<Camera>(new Camera());

static void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
            case GLFW_KEY_W: camera->start_move_forward(); break;
            case GLFW_KEY_S: camera->start_move_backward(); break;
            case GLFW_KEY_A: camera->start_move_left(); break;
            case GLFW_KEY_D: camera->start_move_right(); break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W: camera->stop_move_forward(); break;
            case GLFW_KEY_S: camera->stop_move_backward(); break;
            case GLFW_KEY_A: camera->stop_move_left(); break;
            case GLFW_KEY_D: camera->stop_move_right(); break;
        }
    }
}

static void cursor_pos_callback(GLFWwindow*, double posx, double posy) {
    static double last_posx = posx, last_posy = posy;

    float del_x = static_cast<float>(last_posx - posx);
    float del_y = static_cast<float>(posy - last_posy);
    last_posx = posx;
    last_posy = posy;

    camera->rotate(del_x, del_y);
}

int main() {
    Scene scene = load_map(MAP_PATH);
    vector<GLfloat> vertices = scene.get_vertices();
    vector<GLfloat> uv = scene.get_uv();
    auto texture_data = load_texture(TEXTURE_FOLDER_PATH);
    scene.add_object(camera);

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

    glfwSwapInterval(1); // vsync
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        _exit(1);
    }

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
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint sampler_ID = glGetUniformLocation(program_ID, "sampler");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_ID);
    glUniform1i(sampler_ID, 0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &camera->get_mvp()[0][0]);

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

        scene.move_objects();
    }

    _exit(0);
}
