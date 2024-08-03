#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <vector>
#include <string>

bool initPortAudio();
void listAudioInputDevices(std::vector<std::string>& audioInputList);
void cleanUpPortAudio();

#endif // AUDIO_MANAGER_H
