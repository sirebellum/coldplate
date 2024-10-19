#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <string>
#include <map>
#include <portaudio.h>
#include <sndfile.h>

class SoundPlayer {
public:
    // Constructor and Destructor
    SoundPlayer();
    ~SoundPlayer();

    // Load a sound from a file
    bool loadSound(const std::string& id, const std::string& filePath);

    // Play a loaded sound by ID at a specific volume (0.0 - 1.0)
    bool playSound(const std::string& id, float volume = 1.0f);

    // Stop all currently playing sounds
    void stopAll();

private:
    // Internal structure to hold sound data
    struct SoundData {
        std::vector<float> samples;
        int sampleRate;
        int channels;
    };

    // Map to hold loaded sounds
    std::map<std::string, SoundData> loadedSounds;

    // PortAudio stream
    PaStream* stream;
    static int paCallback(const void* input, void* output, unsigned long frameCount,
                          const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                          void* userData);

    // Helper method to load sound file data
    bool loadSoundFile(const std::string& filePath, SoundData& soundData);
};

#endif // SOUND_PLAYER_H
