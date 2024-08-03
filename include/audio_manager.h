#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <vector>
#include <string>

bool initPortAudio(int deviceIndex);
void listAudioInputDevices(std::vector<std::string>& audioInputList, std::vector<int>& deviceIndices);
void cleanUpPortAudio();

#endif // AUDIO_MANAGER_H
