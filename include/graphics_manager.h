#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include <GLFW/glfw3.h>

extern int lastWidth;
extern int lastHeight;

bool initGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
bool initGLEW();
void runVisualizer(GLFWwindow* window);

// Declare the callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // GRAPHICS_MANAGER_H
