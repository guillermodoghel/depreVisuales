#include "utils.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <GL/glew.h>

bool checkPresetDirectory(const std::string &path) {
    struct stat info{};
    if (stat(path.c_str(), &info) != 0) {
        std::cerr << "Cannot access " << path << std::endl;
        return false;
    } else if (info.st_mode & S_IFDIR) {
        return true;
    } else {
        std::cerr << path << " is not a directory" << std::endl;
        return false;
    }
}

void checkGLError(const std::string &message) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error (" << message << "): " << err << std::endl;
    }
}

void printOpenGLInfo() {
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    checkGLError("after printing OpenGL info");
}
