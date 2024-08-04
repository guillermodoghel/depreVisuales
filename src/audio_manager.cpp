#include "audio_manager.h"
#include "projectm_manager.h"
#include <iostream>
#include <portaudio.h>

#define FRAMES_PER_BUFFER 256

#define SAMPLE_RATE 44100

#include <cmath>  // Include for log10 and pow functions

float gain_dB = 0.0f;  // Gain in decibels, initialized to 0dB

static PaStream *stream = nullptr;
static int sampleRate = SAMPLE_RATE;  // Initialize sample rate with a default value

// Function to convert dB gain to linear gain factor
float dbToGain(float dB) {
    return powf(10.0f, dB / 20.0f);
}

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
    const float *in = (const float *)inputBuffer;
    if (inputBuffer != nullptr && getProjectMHandle() != nullptr) {
        float pcmData[framesPerBuffer * 2];
        float linearGain = dbToGain(gain_dB);  // Convert dB gain to linear gain
        for (unsigned long i = 0; i < framesPerBuffer; ++i) {
            float amplifiedSample = in[i] * linearGain;
            if (amplifiedSample > 1.0f) amplifiedSample = 1.0f;
            if (amplifiedSample < -1.0f) amplifiedSample = -1.0f;
            pcmData[2 * i] = amplifiedSample;
            pcmData[2 * i + 1] = amplifiedSample;
        }
        projectm_pcm_add_float(getProjectMHandle(), pcmData, framesPerBuffer, PROJECTM_STEREO);
    }
    return paContinue;
}




bool initPortAudio(int deviceIndex) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    PaStreamParameters inputParameters;
    inputParameters.device = (deviceIndex != -1) ? deviceIndex : Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        std::cerr << "PortAudio error: No default input device." << std::endl;
        return false;
    }

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(inputParameters.device);
    inputParameters.channelCount = deviceInfo->maxInputChannels;
    if (inputParameters.channelCount < 1) {
        std::cerr << "PortAudio error: Invalid number of channels." << std::endl;
        return false;
    }
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    sampleRate = static_cast<int>(deviceInfo->defaultSampleRate);  // Set sample rate from the device

    err = Pa_OpenStream(&stream, &inputParameters, nullptr, sampleRate, FRAMES_PER_BUFFER, paClipOff, paCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    std::cout << "Audio stream opened with device: " << deviceInfo->name << std::endl;

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    std::cout << "Audio stream started." << std::endl;

    return true;
}

bool setAudioInputDevice(int deviceIndex) {
    if (stream != nullptr) {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }

        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }
        stream = nullptr;
    }

    return initPortAudio(deviceIndex);
}

void listAudioInputDevices(std::vector<std::string>& audioInputList, std::vector<int>& deviceIndices) {
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
            std::string deviceName = std::string(deviceInfo->name) + " (" + hostApiInfo->name + ")";
            audioInputList.push_back(deviceName);
            deviceIndices.push_back(i);
        }
    }

    Pa_Terminate();
}

void cleanUpPortAudio() {
    if (stream != nullptr) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    Pa_Terminate();
}

// Example usage to set gain to +10dB
void setGain(float dB) {
    if (dB < -30.0f) dB = -30.0f;  // Clamp to -30dB
    if (dB > 30.0f) dB = 30.0f;    // Clamp to +30dB
    gain_dB = dB;
}