#include "audio_manager.h"
#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include "settings_window.h"
#include <iostream>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


int main() {
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

    if (!initFramebuffer(lastWidth, lastHeight)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    if (!initProjectM(lastWidth, lastHeight)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int selectedDeviceIndex = audioDeviceIndices.empty() ? -1 : audioDeviceIndices[0];
    if (!initPortAudio(selectedDeviceIndex)) {
        return 1;
    }

    std::vector<std::string> presetList = getPresetList();
    bool shuffleState = false;
    InitializeSettings(presetList, audioInputList, audioDeviceIndices, shuffleState);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Initialize ImGui backend bindings
    std::cout << "Initializing ImGui for GLFW and OpenGL..." << std::endl;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    runVisualizer(window);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    cleanUpPortAudio();
    cleanUpProjectM();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
