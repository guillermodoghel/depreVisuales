#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include <GLFW/glfw3.h>

// Declare the external variables
extern int lastWidth;
extern int lastHeight;

// Function declarations
bool initGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
bool initGLEW();
void runVisualizer(GLFWwindow* window);

#endif // GRAPHICS_MANAGER_H
