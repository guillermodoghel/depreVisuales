#include <GL/glew.h>
#include "graphics_manager.h"
#include "projectm_manager.h"
#include "utils.h"
#include "settings_window.h"
#include <iostream>
#include <vector>
#include <thread>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <chrono>

#include "keyboard.h"


int lastWidth = 800;
int lastHeight = 600;
bool showSettingsWindow = false; // Define the variable

GLuint framebuffer = 0;
GLuint textureColorbuffer = 0;
GLuint VBO, VAO;

// Add texture size tracking
static int currentTextureWidth = 0;
static int currentTextureHeight = 0;

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

void setVSync(bool enable) {
    glfwSwapInterval(enable ? 1 : 0);
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
    setVSync(false); // Disable VSync by default

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

bool initFramebuffer(int width, int height) {
    if (framebuffer == 0) {
        glGenFramebuffers(1, &framebuffer);
        glGenTextures(1, &textureColorbuffer);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    
    // Only reallocate texture if size changed
    if (width != currentTextureWidth || height != currentTextureHeight) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        currentTextureWidth = width;
        currentTextureHeight = height;
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (width == lastWidth && height == lastHeight) {
        return;
    }
    lastWidth = width;
    lastHeight = height;
    glViewport(0, 0, width, height);
    
    // Only update texture if size changed
    if (width != currentTextureWidth || height != currentTextureHeight) {
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        currentTextureWidth = width;
        currentTextureHeight = height;
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if (getProjectMHandle() != nullptr) {
        projectm_set_window_size(getProjectMHandle(), width, height);
    }
}

bool isFrameAllBlack(int width, int height) {
    static const int sampleSize = 64; // Make constant
    static std::vector<GLubyte> pixels(sampleSize * 3);  // Static buffer to avoid reallocations
    static std::vector<std::pair<int, int>> samplePoints(sampleSize);  // Pre-calculate sample points
    static bool initialized = false;
    
    if (!initialized) {
        // Initialize sample points once
        for (int i = 0; i < sampleSize; i++) {
            samplePoints[i].first = i * (width / sampleSize);
            samplePoints[i].second = i * (height / sampleSize);
        }
        initialized = true;
    }
    
    for (int i = 0; i < sampleSize; i++) {
        glReadPixels(samplePoints[i].first, samplePoints[i].second, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixels[i * 3]);
        
        if (pixels[i * 3] != 0 || pixels[i * 3 + 1] != 0 || pixels[i * 3 + 2] != 0) {
            return false;
        }
    }
    return true;
}



void clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void renderFrame() {
    static GLuint lastBoundFB = 0;
    
    if (lastBoundFB != framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        lastBoundFB = framebuffer;
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (getProjectMHandle() != nullptr) {
        projectm_opengl_render_frame(getProjectMHandle());
        checkGLError("during rendering");
    }

    if (lastBoundFB != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        lastBoundFB = 0;
    }
}

void swapBuffersAndPollEvents(GLFWwindow *window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void cleanupFramebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    if (textureColorbuffer) {
        glDeleteTextures(1, &textureColorbuffer);
        textureColorbuffer = 0;
    }
}

void runVisualizer(GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    projectm_set_window_size(getProjectMHandle(), width, height);

    // Frame timing variables
    double lastTime = glfwGetTime();
    int frameCount = 0;
    double fpsUpdateInterval = 0.5; // Update FPS every 0.5 seconds

    try {
        while (!glfwWindowShouldClose(window)) {
            double currentTime = glfwGetTime();
            frameCount++;

            // Calculate FPS every 0.5 seconds
            if (currentTime - lastTime >= fpsUpdateInterval) {
                double fps = frameCount / (currentTime - lastTime);
                std::cout << "FPS: " << fps << std::endl;
                frameCount = 0;
                lastTime = currentTime;
            }

            // Only setup ImGui if settings window is shown
            if (showSettingsWindow) {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
            }

            clearScreen();
            renderFrame();

            if (showSettingsWindow) {
                RenderSettingsWindow(showSettingsWindow);
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                    glfwMakeContextCurrent(window);
                }
            }

            swapBuffersAndPollEvents(window);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error during rendering: " << e.what() << std::endl;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    cleanupFramebuffer();
}

void setupBuffers() {
    float vertices[] = {
        // Full screen quad coordinates
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}
