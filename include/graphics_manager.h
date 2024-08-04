#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include <GLFW/glfw3.h>

extern int lastWidth;
extern int lastHeight;
extern bool showSettingsWindow;

bool initGLFW();
GLFWwindow *createWindow(int width, int height, const char *title);
bool initGLEW();
bool initFramebuffer(int width, int height);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void clearScreen();
void renderFrame();
void swapBuffersAndPollEvents(GLFWwindow *window);
void runVisualizer(GLFWwindow *window);
void cleanupFramebuffer();

#endif // GRAPHICS_MANAGER_H
