#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <portaudio.h>

bool initPortAudio();
void listAudioInputDevices();
void cleanUpPortAudio();

#endif // AUDIO_MANAGER_H
