#include "audio_manager.h"
#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include "settings_window.h"
#include <iostream>
#include <vector>

// Include ImGui backend headers
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int main() {
    // List audio input devices and store them in a vector
    std::vector<std::string> audioInputList;
    listAudioInputDevices(audioInputList);

    if (!initGLFW()) {
        return 1;
    }

    GLFWwindow* window = createWindow(lastWidth, lastHeight, "DepreVisuales");
    if (!window) {
        glfwTerminate();
        return 1;
    }

    if (!initGLEW()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    printOpenGLInfo();

    if (!initProjectM(lastWidth, lastHeight)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    if (!initPortAudio()) {
        return 1;
    }

    // Initialize settings window data
    std::vector<std::string> presetList = getPresetList();
    InitializeSettings(presetList, audioInputList);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    runVisualizer(window);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Clean up resources
    cleanUpPortAudio();
    cleanUpProjectM();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
