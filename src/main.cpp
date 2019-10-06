#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "block.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "ui.hpp"
#include "util.hpp"

using namespace std;

static void load_map(string const& map_path) {
    ifstream map_file(map_path);
    if (!map_file.is_open()) {
        cerr << "Cannot open " << map_path << endl;
        _exit(1);
    }

    while (true) {
        long id, x, y, z;
        map_file >> id >> x >> y >> z;
        if (map_file.eof()) {
            break;
        }
        Block* block = new_block(id, x, y, z);
        if (block != nullptr) {
            Scene::ins().add_block(block);
        }
    }
}

static bool window_exclusive = false;

static void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (window_exclusive) {
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    window_exclusive = false;
                    break;
                case GLFW_KEY_W: Camera::ins().start_move_forward(); break;
                case GLFW_KEY_S: Camera::ins().start_move_backward(); break;
                case GLFW_KEY_A: Camera::ins().start_move_left(); break;
                case GLFW_KEY_D: Camera::ins().start_move_right(); break;
                case GLFW_KEY_SPACE: Camera::ins().jump(); break;
            }
        } else if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_W: Camera::ins().stop_move_forward(); break;
                case GLFW_KEY_S: Camera::ins().stop_move_backward(); break;
                case GLFW_KEY_A: Camera::ins().stop_move_left(); break;
                case GLFW_KEY_D: Camera::ins().stop_move_right(); break;
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

    auto del_x = static_cast<float>(last_posx - posx);
    auto del_y = static_cast<float>(posy - last_posy);
    last_posx = posx;
    last_posy = posy;

    if (window_exclusive) {
        Camera::ins().rotate(del_x, del_y);
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
    if (glfwInit() == 0) {
        _exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        _exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        _exit(1);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSwapInterval(1); // vsync

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderManager::ins().init();
    Scene::ins().init();
    Camera::ins().init();
    UIManager::ins().init();

    load_map(MAP_PATH);
    Scene::ins().add_object(&Camera::ins());

    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    while (glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();

        Scene::ins().move_objects();
        Scene::ins().update_sun_dir();
        Scene::ins().update_vertices();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Scene::ins().render();
        UIManager::ins().render();

        glfwSwapBuffers(window);
    }

    _exit(0);
}
