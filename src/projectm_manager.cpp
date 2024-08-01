#include "projectm_manager.h"
#include "utils.h"
#include <iostream>

#define PRESETS_PATH "/Users/estoussel/dev/depreVisuales/presets"

static projectm_handle projectMHandle = nullptr;
static projectm_playlist_handle playlistHandle = nullptr;

bool initProjectM(int width, int height) {
    projectMHandle = projectm_create();
    if (!projectMHandle) {
        std::cerr << "Failed to initialize projectM" << std::endl;
        return false;
    }

    projectm_set_window_size(projectMHandle, width, height);

    checkGLError("after initializing projectM");

    playlistHandle = projectm_playlist_create(projectMHandle);

    if (!checkPresetDirectory(PRESETS_PATH)) {
        return true;
    }
    std::cout << "Preset directory exists." << std::endl;

    int added = projectm_playlist_add_path(playlistHandle, PRESETS_PATH, true, false);
    if (added == 0) {
        std::cerr << "No presets found in the directory" << std::endl;
        return false;
    }
    std::cout << "Added " << added << " presets to the playlist" << std::endl;

    // TODO enable or disable this
    projectm_playlist_set_shuffle(playlistHandle, true);
    projectm_playlist_play_next(playlistHandle, true);

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
