#ifndef PROJECTM_MANAGER_H
#define PROJECTM_MANAGER_H

#include <string>
#include <vector>
#include <projectM-4/projectM.h>
#include <projectM-4/playlist.h>

extern bool refreshPresets;

void scanPresets(const std::string& directory, std::vector<std::string>& presets);
bool initProjectM(int width, int height);
void cleanUpProjectM();
projectm_handle getProjectMHandle();
projectm_playlist_handle getPlaylistHandle();
const std::vector<std::string>& getPresetList();
std::string getCurrentPreset();
void playNextPreset();
void setCurrentPreset(const std::string& preset);
void setShuffleState(bool enabled);
void updatePresetDuration(float duration, bool enabled);
void presetSwitchedCallback(bool is_hard_cut, unsigned int index, void* user_data);
void setBeatSensitivity(float sensitivity);

#endif // PROJECTM_MANAGER_H
