#ifndef PROJECTM_MANAGER_H
#define PROJECTM_MANAGER_H

#include <projectM-4/projectM.h>
#include <projectM-4/playlist.h>
#include <vector>
#include <string>

bool initProjectM(int width, int height);
void cleanUpProjectM();
projectm_handle getProjectMHandle();
projectm_playlist_handle getPlaylistHandle();
const std::vector<std::string>& getPresetList();

#endif // PROJECTM_MANAGER_H
