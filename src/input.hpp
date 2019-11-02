#ifndef INPUT_HPP
#define INPUT_HPP

#include "opengl.hpp"

void key_callback(GLFWwindow* window, int key, int, int action, int);

void cursor_pos_callback(GLFWwindow* window, double posx, double posy);

void mouse_button_callback(GLFWwindow* window, int button, int action, int);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif
