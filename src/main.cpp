#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "block.hpp"
#include "input.hpp"
#include "opengl.hpp"
#include "player.hpp"
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
        throw new exception();
    }

    while (true) {
        uint16_t type;
        int32_t x, y, z;
        map_file >> type >> x >> y >> z;
        if (map_file.eof()) {
            break;
        }
        BlockID block_id { x, y, z };
        Scene::ins().add_block(block_id, BlockData { type });
    }
}

int main() {
    if (glfwInit() == 0) {
        throw new exception();
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        throw new exception();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw new exception();
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
    Player::ins().init();
    UIManager::ins().init();

    load_map(MAP_PATH);
    Scene::ins().add_object(&Player::ins());

    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    while (glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();

        Scene::ins().update();
        Player::ins().update();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Scene::ins().render();
        UIManager::ins().render();

        glfwSwapBuffers(window);
    }

    UIManager::ins().shutdown();
    Scene::ins().shutdown();

    return 0;
}
