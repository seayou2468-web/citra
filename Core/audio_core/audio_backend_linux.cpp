// Linux Audio Backend using ALSA
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#include <alsa/asoundlib.h>
#include <thread>
#include <atomic>
#include "audio_core/audio_backend.h"
#include "common/logging/log.h"

namespace AudioCore {

class ALSAAudioBackend : public AudioBackend {
private:
    snd_pcm_t* pcm_handle = nullptr;
    unsigned int sample_rate = 0;
    std::function<void(s16*, std::size_t)> callback;
    std::thread playback_thread;
    std::atomic<bool> running{false};

    void PlaybackThreadFunc();

public:
    ~ALSAAudioBackend() override {
        Stop();
        if (pcm_handle) {
            snd_pcm_close(pcm_handle);
        }
    }

    bool Initialize(const std::string& device_name) override {
        const char* dev = device_name.empty() ? "default" : device_name.c_str();
        
        if (snd_pcm_open(&pcm_handle, dev, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
            LOG_ERROR(Audio_Sink, "Failed to open ALSA device: {}", dev);
            return false;
        }

        snd_pcm_hw_params_t* hw_params;
        snd_pcm_hw_params_alloca(&hw_params);

        if (snd_pcm_hw_params_any(pcm_handle, hw_params) < 0) {
            LOG_ERROR(Audio_Sink, "Failed to initialize ALSA hardware parameters");
            snd_pcm_close(pcm_handle);
            pcm_handle = nullptr;
            return false;
        }

        // Configure: S16 LE, stereo, 32000 Hz
        snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(pcm_handle, hw_params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(pcm_handle, hw_params, 2);
        
        sample_rate = 32000;
        snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &sample_rate, nullptr);

        if (snd_pcm_hw_params(pcm_handle, hw_params) < 0) {
            LOG_ERROR(Audio_Sink, "Failed to set ALSA hardware parameters");
            snd_pcm_close(pcm_handle);
            pcm_handle = nullptr;
            return false;
        }

        snd_pcm_prepare(pcm_handle);
        return true;
    }

    unsigned int GetNativeSampleRate() const override {
        return sample_rate;
    }

    void SetCallback(std::function<void(s16*, std::size_t)> cb) override {
        callback = cb;
    }

    bool Start() override {
        if (running || !pcm_handle) return false;
        running = true;
        playback_thread = std::thread(&ALSAAudioBackend::PlaybackThreadFunc, this);
        return true;
    }

    void Stop() override {
        running = false;
        if (playback_thread.joinable()) {
            playback_thread.join();
        }
    }
};

void ALSAAudioBackend::PlaybackThreadFunc() {
    const int frame_size = 512;
    s16 buffer[frame_size * 2]; // stereo

    while (running && pcm_handle) {
        if (callback) {
            callback(buffer, frame_size);
        }

        snd_pcm_sframes_t frames = snd_pcm_writei(pcm_handle, buffer, frame_size);
        if (frames < 0) {
            frames = snd_pcm_recover(pcm_handle, frames, 0);
            if (frames < 0) {
                LOG_ERROR(Audio_Sink, "ALSA write error");
                break;
            }
        }
    }
}

std::vector<std::string> AudioBackend::ListDevices() {
    std::vector<std::string> devices;
    devices.push_back("default");
    
    // TODO: Enumerate ALSA devices properly
    return devices;
}

std::unique_ptr<AudioBackend> AudioBackend::Create() {
    return std::make_unique<ALSAAudioBackend>();
}

} // namespace AudioCore
