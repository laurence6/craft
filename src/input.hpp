#ifndef INPUT_HPP
#define INPUT_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int, int action, int);

void cursor_pos_callback(GLFWwindow*, double posx, double posy);

void mouse_button_callback(GLFWwindow* window, int button, int action, int);

#endif
