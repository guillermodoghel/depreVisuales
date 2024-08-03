#include <GL/glew.h>  // Ensure GLEW is included before any OpenGL headers
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
            projectm_playlist_play_next(getPlaylistHandle(), false);
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

    if (deltaTime >= 1.0) {
        double fps = static_cast<double>(frameCount) / deltaTime;

        std::string title = "DepreVisuales - FPS: " + std::to_string(fps);
        glfwSetWindowTitle(window, title.c_str());

        // Check if FPS is below 30 and switch preset if it is
        if (fps < 30.0) {
            if (getProjectMHandle() != nullptr) {
                projectm_playlist_play_next(getPlaylistHandle(), true);
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

bool isFrameBlack(int width, int height, int sampleRate = 1) {
    // Calculate sample points (higher resolution by reducing the divisor)
    int checkWidth = width / sampleRate;
    int checkHeight = height / sampleRate;
    std::vector<GLubyte> pixels(checkWidth * checkHeight * 3); // 3 bytes per pixel (RGB)

    glFinish(); // Ensure all previous OpenGL commands are done

    // Bind the default framebuffer (0)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Read pixels from the framebuffer at a higher resolution
    glReadPixels(0, 0, checkWidth, checkHeight, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);

    for (int i = 0; i < checkWidth * checkHeight; ++i) {
        int pixelIndex = i * 3;
        if (pixels[pixelIndex] != 0 || pixels[pixelIndex + 1] != 0 || pixels[pixelIndex + 2] != 0) {
            return false;
        }
    }

    return true;
}

void playNextPresetIfItsAllBlack(int frameCounter, int width, int height) {
    static std::vector<bool> blackFrameBuffer(20, false);
    static int currentIndex = 0;

    if (frameCounter % 30 == 0) { // Check every 10 frames
        bool isBlack = isFrameBlack(width, height, 1); // Higher resolution by setting sampleRate to 1
        blackFrameBuffer[currentIndex] = isBlack;

        currentIndex = (currentIndex + 1) % blackFrameBuffer.size();

        if (std::all_of(blackFrameBuffer.begin(), blackFrameBuffer.end(), [](bool v) { return v; })) {
            std::cout << "Screen is all black for the last 10 checks, switching to next preset." << std::endl;
            if (getProjectMHandle() != nullptr) {
                playNextPreset();
            }

            // Reset the buffer
            std::fill(blackFrameBuffer.begin(), blackFrameBuffer.end(), false);
            currentIndex = 0;
        }
    }
}

void runVisualizer(GLFWwindow* window) {
    try {
        int frameCounter = 0;
        bool isFirstRender = true;

        while (!glfwWindowShouldClose(window)) {
            // Start the ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            clearScreen();
            showFPS(window);
            handleFirstRender(window, isFirstRender);
            renderFrame();
            // playNextPresetIfItsAllBlack(frameCounter, lastWidth, lastHeight);
            frameCounter++;

            // Render the settings window if needed
            RenderSettingsWindow(showSettingsWindow);

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            swapBuffersAndPollEvents(window);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error during rendering: " << e.what() << std::endl;
    }
}
