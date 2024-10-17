#include "Speaker.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

SoundPlayer::SoundPlayer(const char* device_name, unsigned int rate)
    : pcm_device_name(device_name), sample_rate(rate), pcm_handle(nullptr) {
    initPCM();
}

SoundPlayer::~SoundPlayer() {
    closePCM();
}

void SoundPlayer::initPCM() {
    int rc;
    snd_pcm_hw_params_t *params;

    rc = snd_pcm_open(&pcm_handle, pcm_device_name, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        throw std::runtime_error("Unable to open PCM device: " + std::string(snd_strerror(rc)));
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, 1); // Mono sound
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, 0);

    rc = snd_pcm_hw_params(pcm_handle, params);
    if (rc < 0) {
        throw std::runtime_error("Unable to set PCM hardware parameters: " + std::string(snd_strerror(rc)));
    }
}

void SoundPlayer::closePCM() {
    if (pcm_handle) {
        snd_pcm_drain(pcm_handle);
        snd_pcm_close(pcm_handle);
        pcm_handle = nullptr;
    }
}

std::vector<int16_t> SoundPlayer::generateSineWave(double frequency, unsigned int duration_ms) {
    std::vector<int16_t> buffer;
    int num_samples = static_cast<int>((sample_rate * duration_ms) / 1000);
    double phase_step = (2.0 * M_PI * frequency) / sample_rate;

    for (int i = 0; i < num_samples; ++i) {
        double sample = sin(i * phase_step);
        buffer.push_back(static_cast<int16_t>(sample * 32767)); // Scale to 16-bit
    }

    return buffer;
}

void SoundPlayer::playTone(double frequency, unsigned int duration_ms) {
    std::vector<int16_t> sine_wave = generateSineWave(frequency, duration_ms);
    playSound(sine_wave);
}

void SoundPlayer::playSound(const std::vector<int16_t>& sound_buffer) {
    int rc = snd_pcm_writei(pcm_handle, sound_buffer.data(), sound_buffer.size());
    if (rc == -EPIPE) {
        snd_pcm_prepare(pcm_handle);
        snd_pcm_writei(pcm_handle, sound_buffer.data(), sound_buffer.size());
    } else if (rc < 0) {
        throw std::runtime_error("Error writing to PCM device: " + std::string(snd_strerror(rc)));
    }
}
