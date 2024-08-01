#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool initGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
bool initGLEW();
void runVisualizer(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // GRAPHICS_MANAGER_H
