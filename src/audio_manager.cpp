#include "audio_manager.h"
#include "projectm_manager.h"
#include <iostream>

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 256
#define GAIN 10.0

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
    const float *in = (const float *)inputBuffer;
    if (inputBuffer != nullptr && getProjectMHandle() != nullptr) {
        float pcmData[framesPerBuffer * 2];
        for (unsigned long i = 0; i < framesPerBuffer; ++i) {
            float amplifiedSample = in[i] * GAIN;
            if (amplifiedSample > 1.0f) amplifiedSample = 1.0f;
            if (amplifiedSample < -1.0f) amplifiedSample = -1.0f;
            pcmData[2 * i] = amplifiedSample;
            pcmData[2 * i + 1] = amplifiedSample;
        }
        projectm_pcm_add_float(getProjectMHandle(), pcmData, framesPerBuffer, PROJECTM_STEREO);
    }
    return paContinue;
}

bool initPortAudio() {
    PaStream *stream;
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, paCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    std::cout << "Default input stream opened." << std::endl;

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    std::cout << "Audio stream started." << std::endl;

    return true;
}

void listAudioInputDevices() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(numDevices) << std::endl;
        return;
    }

    const PaDeviceInfo *deviceInfo;
    const PaHostApiInfo *hostApiInfo;
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0) {
            hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            std::cout << "Device " << i << ": " << deviceInfo->name << " (" << hostApiInfo->name << ")" << std::endl;
            std::cout << "    Max input channels: " << deviceInfo->maxInputChannels << std::endl;
            std::cout << "    Default sample rate: " << deviceInfo->defaultSampleRate << std::endl;
        }
    }

    Pa_Terminate();
}

void cleanUpPortAudio() {
    Pa_Terminate();
}
