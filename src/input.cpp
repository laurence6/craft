#include "input.hpp"
#include "player.hpp"
#include "scene.hpp"

static bool window_exclusive = false;

void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (window_exclusive) {
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    window_exclusive = false;
                    break;
                case GLFW_KEY_W: Player::ins().start_move_forward(); break;
                case GLFW_KEY_S: Player::ins().start_move_backward(); break;
                case GLFW_KEY_A: Player::ins().start_move_left(); break;
                case GLFW_KEY_D: Player::ins().start_move_right(); break;
                case GLFW_KEY_SPACE: Player::ins().jump(); break;
            }
        } else if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_W: Player::ins().stop_move_forward(); break;
                case GLFW_KEY_S: Player::ins().stop_move_backward(); break;
                case GLFW_KEY_A: Player::ins().stop_move_left(); break;
                case GLFW_KEY_D: Player::ins().stop_move_right(); break;
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

void cursor_pos_callback(GLFWwindow*, double posx, double posy) {
    static double last_posx = posx, last_posy = posy;

    auto del_x = static_cast<float>(last_posx - posx);
    auto del_y = static_cast<float>(posy - last_posy);
    last_posx = posx;
    last_posy = posy;

    if (window_exclusive) {
        Player::ins().rotate(del_x, del_y);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    if (window_exclusive) {
        if (action == GLFW_RELEASE) {
            switch (button) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    if (Player::ins().target != nullptr) {
                        Scene::ins().block_manager.del_block(Player::ins().target);
                    }
                break;
            }
        }
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
