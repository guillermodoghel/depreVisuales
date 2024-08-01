#ifndef PROJECTM_MANAGER_H
#define PROJECTM_MANAGER_H

#include <projectM-4/projectM.h>
#include <projectM-4/playlist.h>

bool initProjectM(int width, int height);
void cleanUpProjectM();
projectm_handle getProjectMHandle();
projectm_playlist_handle getPlaylistHandle();

#endif // PROJECTM_MANAGER_H
