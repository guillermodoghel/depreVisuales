#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <vector>
#include <string>

extern float gain;  // Declare the gain variable

bool initPortAudio(int deviceIndex);
bool setAudioInputDevice(int deviceIndex);
void listAudioInputDevices(std::vector<std::string>& audioInputList, std::vector<int>& deviceIndices);
void cleanUpPortAudio();
void setGain(float newGain);  // Declare the function to set gain

#endif // AUDIO_MANAGER_H
