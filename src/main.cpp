#include "audio_manager.h"
#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include "settings_window.h"
#include <iostream>

// Include ImGui backend headers
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int main() {
    // List audio input devices
    std::vector<std::string> audioInputList;
    std::vector<int> audioDeviceIndices;
    listAudioInputDevices(audioInputList, audioDeviceIndices);

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

    // Select the first audio device by default
    int selectedDeviceIndex = audioDeviceIndices.empty() ? -1 : audioDeviceIndices[0];
    if (!initPortAudio(selectedDeviceIndex)) {
        return 1;
    }

    // Initialize settings window data
    std::vector<std::string> presetList = getPresetList();
    bool shuffleState = false;  // Shuffle is disabled by default
    InitializeSettings(presetList, audioInputList, audioDeviceIndices, shuffleState);

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
