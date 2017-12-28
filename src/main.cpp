#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "block.hpp"
#include "camera.hpp"
#include "render.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "util.hpp"

using namespace std;

const string VERTEX_SHADER_PATH = "shader/vertexshader.glsl";
const string FRAGMENT_SHADER_PATH = "shader/fragmentshader.glsl";
const string TEXTURE_FOLDER_PATH = "texture";
const string MAP_PATH = "map";

static void load_map(string map_path) {
    ifstream map_file(map_path);
    if (!map_file.is_open()) {
        cerr << "Cannot open " << map_path << endl;
        _exit(1);
    }

    while (true) {
        string t; long x, y; unsigned int z;
        map_file >> t >> x >> y >> z;
        if (map_file.eof()) {
            break;
        }
        Block* block = nullptr;
        if (t == "dirt") {
            block = new DirtBlock(x, y, z);
        } else if (t == "grass") {
            block = new GrassBlock(x, y, z);
        } else if (t == "stone") {
            block = new StoneBlock(x, y, z);
        }
        if (block != nullptr) {
            Scene::instance().add_block(block);
        }
    }

    Scene::instance().update_vertices_uv();
}

static Camera* camera = new Camera();

static bool window_exclusive = false;

static void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (window_exclusive) {
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    window_exclusive = false;
                    break;
                case GLFW_KEY_W: camera->start_move_forward(); break;
                case GLFW_KEY_S: camera->start_move_backward(); break;
                case GLFW_KEY_A: camera->start_move_left(); break;
                case GLFW_KEY_D: camera->start_move_right(); break;
                case GLFW_KEY_SPACE: camera->jump(); break;
            }
        } else if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_W: camera->stop_move_forward(); break;
                case GLFW_KEY_S: camera->stop_move_backward(); break;
                case GLFW_KEY_A: camera->stop_move_left(); break;
                case GLFW_KEY_D: camera->stop_move_right(); break;
            }
        }
    } else {
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
            }
        }
    }
}

static void cursor_pos_callback(GLFWwindow*, double posx, double posy) {
    static double last_posx = posx, last_posy = posy;

    float del_x = static_cast<float>(last_posx - posx);
    float del_y = static_cast<float>(posy - last_posy);
    last_posx = posx;
    last_posy = posy;

    if (window_exclusive) {
        camera->rotate(del_x, del_y);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    if (window_exclusive) {
    } else {
        if (action == GLFW_RELEASE) {
            switch (button) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    window_exclusive = true;
                    break;
            }
        }
    }
}

int main() {
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

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        _exit(1);
    }

    glfwSwapInterval(1); // vsync

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint vertex_array_ID;
    glGenVertexArrays(1, &vertex_array_ID);
    glBindVertexArray(vertex_array_ID);

    GLuint program_ID = load_shaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    glUseProgram(program_ID);

    GLuint matrix_ID = glGetUniformLocation(program_ID, "MVP");

    GLuint texture_ID;
    {
        auto texture_data = load_texture(TEXTURE_FOLDER_PATH);

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
    }

    GLuint sampler_ID = glGetUniformLocation(program_ID, "sampler");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_ID);
    glUniform1i(sampler_ID, 0);

    RenderManager::instance().init();
    Scene::instance().init();

    load_map(MAP_PATH);
    Scene::instance().add_object(camera);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        Scene::instance().move_objects();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &camera->get_mvp()[0][0]);

        RenderManager::instance().render();

        glfwSwapBuffers(window);
    }

    _exit(0);
}
