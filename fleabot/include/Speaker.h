#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <alsa/asoundlib.h>
#include <vector>
#include <cstdint>

class SoundPlayer {
public:
    SoundPlayer(const char* device_name, unsigned int sample_rate);
    ~SoundPlayer();

    // Plays a tone at the given frequency for a certain duration (milliseconds)
    void playTone(double frequency, unsigned int duration_ms);

    // Plays a custom sound from a buffer of samples
    void playSound(const std::vector<int16_t>& sound_buffer);

private:
    // Initializes the ALSA PCM device
    void initPCM();

    // Close the ALSA PCM device
    void closePCM();

    // Generates a sine wave buffer for the given frequency and duration
    std::vector<int16_t> generateSineWave(double frequency, unsigned int duration_ms);

    const char* pcm_device_name;
    unsigned int sample_rate;
    snd_pcm_t* pcm_handle; // Handle for the ALSA PCM device
};

#endif // SOUND_PLAYER_H
