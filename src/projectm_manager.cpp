#include "projectm_manager.h"
#include "utils.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>  // Include to handle std::bad_alloc

#ifndef INFINITY
#define INFINITY ((float)(1e+300 * 1e+300))
#endif

const std::string presetsPath = PRESETS_PATH;
const std::string texturesPath = TEXTURES_PATH;

static projectm_handle projectMHandle = nullptr;
static projectm_playlist_handle playlistHandle = nullptr;
static std::vector<std::string> presetList;
static std::string currentPreset; // Variable to store the current preset name
static bool shuffleEnabled = false; // Initialize shuffle state to false
bool refreshPresets = false; // Define the variable here

namespace fs = std::__fs::filesystem; // Ensure correct namespace for filesystem

void scanPresets(const std::string &directory, std::vector<std::string> &presets) {
    try {
        for (const auto &entry: fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".milk") {
                presets.push_back(entry.path().string());
            }
        }
        std::sort(presets.begin(), presets.end());
    } catch (const std::bad_alloc &e) {
        std::cerr << "Memory allocation error: " << e.what() << std::endl;
        throw;
    }
}

bool initProjectM(int width, int height) {
    projectMHandle = projectm_create();
    if (!projectMHandle) {
        std::cerr << "Failed to initialize projectM" << std::endl;
        return false;
    }

    projectm_set_window_size(projectMHandle, width, height);
    checkGLError("after initializing projectM");

    playlistHandle = projectm_playlist_create(projectMHandle);

    if (!checkPresetDirectory(presetsPath.c_str())) {
        return true;
    }
    std::cout << "Preset directory exists." << std::endl;

    projectm_set_preset_duration(projectMHandle, INFINITY);
    projectm_set_fps(projectMHandle, 120);
    projectm_playlist_set_retry_count(playlistHandle, 0);

    const char* texture_paths[] = {texturesPath.c_str()};
    size_t path_count = 1;

    projectm_set_texture_search_paths(projectMHandle, texture_paths, path_count);

    int added = projectm_playlist_add_path(playlistHandle, presetsPath.c_str(), true, false);
    if (added == 0) {
        std::cerr << "No presets found in the directory" << std::endl;
        return false;
    }
    std::cout << "Added " << added << " presets to the playlist" << std::endl;

    // Scan the presets for the settings window
    try {
        scanPresets(presetsPath.c_str(), presetList);
    } catch (const std::bad_alloc &e) {
        std::cerr << "Failed to scan presets: " << e.what() << std::endl;
        return false;
    }

    currentPreset = presetList.empty() ? "No presets available" : presetList[0]; // Initialize the current preset

    projectm_playlist_set_preset_switched_event_callback(playlistHandle, presetSwitchedCallback, nullptr);

    return true;
}

void cleanUpProjectM() {
    if (projectMHandle) {
        projectm_destroy(projectMHandle);
    }
}

projectm_handle getProjectMHandle() {
    return projectMHandle;
}

projectm_playlist_handle getPlaylistHandle() {
    return playlistHandle;
}

const std::vector<std::string> &getPresetList() {
    return presetList;
}

std::string getCurrentPreset() {
    return currentPreset;
}

void playNextPreset() {
    if (playlistHandle != nullptr) {
        projectm_playlist_play_next(playlistHandle, false);
        // Update the current preset to the next one in the list
        auto it = std::find(presetList.begin(), presetList.end(), currentPreset);
        if (it != presetList.end() && ++it != presetList.end()) {
            currentPreset = *it;
        } else if (!presetList.empty()) {
            currentPreset = presetList[0];
        }
    }
}

void setCurrentPreset(const std::string &preset) {
    if (playlistHandle != nullptr) {
        auto it = std::find(presetList.begin(), presetList.end(), preset);
        if (it != presetList.end()) {
            uint32_t index = std::distance(presetList.begin(), it);
            projectm_playlist_set_position(playlistHandle, index, true);
            currentPreset = preset;
        }
    }
}

void setShuffleState(bool enabled) {
    if (playlistHandle != nullptr) {
        projectm_playlist_set_shuffle(playlistHandle, enabled);
        shuffleEnabled = enabled;
    }
}

void updatePresetDuration(float duration, bool enabled) {
    if (projectMHandle != nullptr) {
        projectm_set_preset_duration(projectMHandle, enabled ? duration : INFINITY);
    }
}

void presetSwitchedCallback(bool is_hard_cut, unsigned int index, void *user_data) {
    if (index < presetList.size()) {
        currentPreset = presetList[index];
    }
    refreshPresets = true; // Set the flag to force refresh
}

void setBeatSensitivity(float sensitivity) {
    if (projectMHandle != nullptr) {
        projectm_set_beat_sensitivity(projectMHandle, sensitivity);
    }
}
