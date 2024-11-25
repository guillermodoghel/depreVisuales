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
    glfwSwapInterval(1); // Enable VSync

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
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

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
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (getProjectMHandle() != nullptr) {
        projectm_set_window_size(getProjectMHandle(), width, height);
    }
}

bool isFrameAllBlack(int width, int height) {
    const int sampleSize = 64; // Check fewer pixels
    std::vector<GLubyte> pixels(sampleSize * 3);
    
    for (int i = 0; i < sampleSize; i++) {
        int x = rand() % width;
        int y = rand() % height;
        glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixels[i * 3]);
        
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
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (getProjectMHandle() != nullptr) {
        projectm_opengl_render_frame(getProjectMHandle());
        checkGLError("during rendering");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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



    try {
        while (!glfwWindowShouldClose(window)) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            clearScreen();
            renderFrame();


            if (showSettingsWindow) {
                RenderSettingsWindow(showSettingsWindow);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(window);
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
