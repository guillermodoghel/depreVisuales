#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include <iostream>
#include <GL/glew.h>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>

int lastWidth = 800;
int lastHeight = 600;

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

GLFWwindow *createWindow(int width, int height, const char *title) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (getProjectMHandle() != nullptr) {
            projectm_playlist_play_next(getPlaylistHandle(), true);
        }
    }
}

void showFPS(GLFWwindow *window) {
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

void handleFirstRender(GLFWwindow* window, bool &isFirstRender) {
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

void swapBuffersAndPollEvents(GLFWwindow *window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool isScreenAllBlack(int width, int height) {
    // Allocate memory to read pixels at a lower resolution
    int checkWidth = width / 10;
    int checkHeight = height / 10;
    std::vector<GLubyte> pixels(checkWidth * checkHeight); // 1 byte per pixel (using GL_RED)

    // Read pixels from the framebuffer at a lower resolution
    glReadPixels(0, 0, checkWidth, checkHeight, GL_RED, GL_UNSIGNED_BYTE, &pixels[0]);

    // Check if all pixels are black using multiple threads
    std::atomic<bool> allBlack(true);
    int numThreads = std::thread::hardware_concurrency();
    int blockSize = pixels.size() / numThreads;

    auto checkBlock = [&](int start, int end) {
        for (int i = start; i < end; ++i) {
            if (pixels[i] != 0) {
                allBlack.store(false);
                return;
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * blockSize;
        int end = (i == numThreads - 1) ? pixels.size() : (i + 1) * blockSize;
        threads.emplace_back(checkBlock, start, end);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    return allBlack.load();
}

void playNextPresetIfItsAllBlack(int frameCounter ) {
    if (frameCounter % 30 == 0) {
        if (isScreenAllBlack(lastWidth, lastHeight)) {
            std::cout << "Screen is all black, switching to next preset." << std::endl;
            if (getProjectMHandle() != nullptr) {
                projectm_playlist_play_next(getPlaylistHandle(), true);
            }
        }
    }
}

void runVisualizer(GLFWwindow *window) {
    try {
        int frameCounter = 0;

        bool isFirstRender = true;
        while (!glfwWindowShouldClose(window)) {
            clearScreen();
            showFPS(window);
            handleFirstRender(window, isFirstRender);
            renderFrame();
            playNextPresetIfItsAllBlack(frameCounter);
            frameCounter++;

            swapBuffersAndPollEvents(window);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error during rendering: " << e.what() << std::endl;
    }
}

