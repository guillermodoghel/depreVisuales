cmake_minimum_required(VERSION 3.10)
project(DepreVisuales)

set(CMAKE_CXX_STANDARD 14)

# Add custom modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)

set(PROJECTM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/projectm/bin/include)
set(PROJECTM_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/dependencies/projectm/bin/lib)
#set(PRESETS_PATH_DEFAULT "${CMAKE_SOURCE_DIR}/dependencies/projectm/presets")
set(PRESETS_PATH_DEFAULT "${CMAKE_SOURCE_DIR}/presets")
add_definitions(-DPRESETS_PATH=\"${PRESETS_PATH_DEFAULT}\")

# Find the projectM library
find_library(PROJECTM_LIBRARY NAMES projectM-4 PATHS ${PROJECTM_LIBRARY_DIR})

if (NOT PROJECTM_LIBRARY)
    message(FATAL_ERROR "Could not find the projectM library")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)

# Manually specify PortAudio paths
set(PORTAUDIO_INCLUDE_DIRS /opt/homebrew/include)
set(PORTAUDIO_LIBRARIES /opt/homebrew/lib/libportaudio.dylib)

# Include directories
include_directories(${PROJECTM_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${PORTAUDIO_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Include directories for Dear ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/dependencies/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

# Link directories
link_directories(${PROJECTM_LIBRARY_DIR})

# Set the RPATH to include the necessary directories
set(CMAKE_BUILD_RPATH "${PROJECTM_LIBRARY_DIR};/usr/local/lib;/opt/homebrew/lib")
set(CMAKE_INSTALL_RPATH "${PROJECTM_LIBRARY_DIR};/usr/local/lib;/opt/homebrew/lib")

# Ensure RPATH is used when installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Add source files
set(SOURCES
        src/main.cpp
        src/audio_manager.cpp
        src/graphics_manager.cpp
        src/projectm_manager.cpp
        src/utils.cpp
        src/listAudioDevices.cpp
        src/settings_window.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add header files
set(HEADERS
        include/audio_manager.h
        include/graphics_manager.h
        include/projectm_manager.h
        include/utils.h
        include/settings_window.h
)

# Add executable and link libraries
add_executable(DepreVisuales ${SOURCES} ${HEADERS})
target_link_libraries(DepreVisuales
        ${PROJECTM_LIBRARY}
        ${PORTAUDIO_LIBRARIES}
        ${OPENGL_LIBRARIES}
        glfw
        GLEW::GLEW
        ${FREETYPE_LIBRARIES}
        ${PROJECTM_LIBRARY_DIR}/libprojectM-4.4.1.1.dylib
        ${PROJECTM_LIBRARY_DIR}/libprojectM-4-playlist.4.1.1.dylib
)
