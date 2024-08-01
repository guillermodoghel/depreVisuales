#include "audio_manager.h"
#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include "settings_window.h"
#include <iostream>

// Include ImGui backend headers
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Declare the external variables
extern int lastWidth;
extern int lastHeight;

int main() {
    // List audio input devices
    listAudioInputDevices();

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
    std::vector<std::string> presetList = {"Cras justo odio", "Dapibus ac facilisis in", "Morbi leo risus", "Porta ac consectetur ac", "Vestibulum at eros"};
    std::vector<std::string> audioInputList = {"Input 1", "Input 2", "Input 3"};
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
