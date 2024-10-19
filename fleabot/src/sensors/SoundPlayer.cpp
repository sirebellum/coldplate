#include "SoundPlayer.h"
#include <iostream>
#include <vector>

SoundPlayer::SoundPlayer() : stream(nullptr) {
    Pa_Initialize();
}

SoundPlayer::~SoundPlayer() {
    stopAll();
    Pa_Terminate();
}

bool SoundPlayer::loadSound(const std::string& id, const std::string& filePath) {
    SoundData soundData;
    if (loadSoundFile(filePath, soundData)) {
        loadedSounds[id] = soundData;
        return true;
    }
    return false;
}

bool SoundPlayer::playSound(const std::string& id, float volume) {
    auto it = loadedSounds.find(id);
    if (it == loadedSounds.end()) {
        std::cerr << "Sound ID not found: " << id << std::endl;
        return false;
    }

    SoundData& soundData = it->second;
    std::vector<float> volumeAdjustedSamples = soundData.samples;

    // Adjust volume
    for (auto& sample : volumeAdjustedSamples) {
        sample *= volume;
    }

    // Stop any existing stream before starting a new one
    stopAll();

    // Open PortAudio stream
    PaError err = Pa_OpenDefaultStream(&stream, 0, soundData.channels, paFloat32,
                                       soundData.sampleRate, paFramesPerBufferUnspecified,
                                       paCallback, &volumeAdjustedSamples);
    if (err != paNoError) {
        std::cerr << "Failed to open PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    // Start playback
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    return true;
}

void SoundPlayer::stopAll() {
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
}

int SoundPlayer::paCallback(const void* input, void* output, unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                            void* userData) {
    auto* samples = static_cast<std::vector<float>*>(userData);
    float* out = static_cast<float*>(output);

    static size_t currentSampleIndex = 0;
    for (unsigned long i = 0; i < frameCount; ++i) {
        if (currentSampleIndex < samples->size()) {
            *out++ = (*samples)[currentSampleIndex++];
        } else {
            *out++ = 0.0f; // Fill with silence when done
        }
    }

    return currentSampleIndex < samples->size() ? paContinue : paComplete;
}

bool SoundPlayer::loadSoundFile(const std::string& filePath, SoundData& soundData) {
    SF_INFO sfInfo;
    SNDFILE* sndFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!sndFile) {
        std::cerr << "Failed to load sound file: " << filePath << std::endl;
        return false;
    }

    soundData.sampleRate = sfInfo.samplerate;
    soundData.channels = sfInfo.channels;
    soundData.samples.resize(sfInfo.frames * sfInfo.channels);
    sf_read_float(sndFile, soundData.samples.data(), sfInfo.frames * sfInfo.channels);
    sf_close(sndFile);

    return true;
}
