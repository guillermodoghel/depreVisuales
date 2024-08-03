#include <GL/glew.h>
#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include "settings_window.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int lastWidth = 800;
int lastHeight = 600;

GLuint framebuffer = 0;
GLuint textureColorbuffer = 0;

bool initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

GLFWwindow* createWindow(int width, int height, const char* title) {
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    return window;
}

bool initGLEW() {
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewStatus) << std::endl;
        return false;
    }
    return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width == lastWidth && height == lastHeight) {
        return;
    }
    lastWidth = width;
    lastHeight = height;
    glViewport(0, 0, width, height);
    if (getProjectMHandle() != nullptr) {
        projectm_set_window_size(getProjectMHandle(), width, height);
    }
}

bool showSettingsWindow = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (getProjectMHandle() != nullptr) {
            playNextPreset();
        }
    }
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        showSettingsWindow = !showSettingsWindow;
    }
}

void showFPS(GLFWwindow* window) {
    static int frameCount = 0;
    static double previousTime = glfwGetTime();

    double currentTime = glfwGetTime();
    double deltaTime = currentTime - previousTime;
    frameCount++;

    if (deltaTime >= 2.0) {
        double fps = static_cast<double>(frameCount) / deltaTime;

        std::string title = "DepreVisuales - FPS: " + std::to_string(fps);
        glfwSetWindowTitle(window, title.c_str());

        // Check if FPS is below 30 and switch preset if it is
        if (fps < 15.0) {
            if (getProjectMHandle() != nullptr) {
                playNextPreset();
            }
        }

        frameCount = 0;
        previousTime = currentTime;
    }
}

void clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void handleFirstRender(GLFWwindow* window, bool& isFirstRender) {
    if (isFirstRender && getProjectMHandle() != nullptr) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        projectm_set_window_size(getProjectMHandle(), width, height);
        isFirstRender = false;
    }
}

void renderFrame() {
    if (getProjectMHandle() != nullptr) {
        projectm_opengl_render_frame(getProjectMHandle());
        checkGLError("during rendering");
    }
}

void swapBuffersAndPollEvents(GLFWwindow* window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}


void runVisualizer(GLFWwindow* window) {
    int frameCounter = 0;
    bool isFirstRender = true;
    GLFWmonitor* lastMonitor = glfwGetWindowMonitor(window);

    try {
        while (!glfwWindowShouldClose(window)) {
            auto startTime = std::chrono::high_resolution_clock::now();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            clearScreen();
            showFPS(window);
            handleFirstRender(window, isFirstRender);
            renderFrame();
            frameCounter++;

            RenderSettingsWindow(showSettingsWindow);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Detect monitor changes
            GLFWmonitor* currentMonitor = glfwGetWindowMonitor(window);
            if (currentMonitor != lastMonitor) {
                std::cout << "Monitor changed. Reinitializing context." << std::endl;
                // Reinitialize context or take appropriate action
                glfwMakeContextCurrent(window);
                lastMonitor = currentMonitor;
            }

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(window);
            }

            swapBuffersAndPollEvents(window);

            // Calculate frame time and sleep to limit frame rate
            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> frameDuration = endTime - startTime;
            double frameTime = frameDuration.count();
            double targetFrameTime = 1.0 / 60.0;  // Target 60 FPS
            if (frameTime < targetFrameTime) {
                std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - frameTime));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during rendering: " << e.what() << std::endl;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}