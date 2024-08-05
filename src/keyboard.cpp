#include "../include/keyboard.h"


#include "graphics_manager.h"
#include "projectm_manager.h"



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (getProjectMHandle() != nullptr) {
            playNextPreset();
        }
    }
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        showSettingsWindow = !showSettingsWindow;
    }
}
