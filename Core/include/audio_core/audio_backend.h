#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "audio_core/audio_types.h"

namespace AudioCore {

/**
 * Abstract audio backend interface
 * Platform-specific implementations: iOS (AVAudioEngine), Linux (ALSA/PulseAudio)
 */
class AudioBackend {
public:
    virtual ~AudioBackend() = default;

    /// Initialize audio backend with device name
    virtual bool Initialize(const std::string& device_name) = 0;

    /// Get native sample rate
    virtual unsigned int GetNativeSampleRate() const = 0;

    /// Set audio callback for rendering samples
    virtual void SetCallback(std::function<void(s16*, std::size_t)> callback) = 0;

    /// Start audio playback
    virtual bool Start() = 0;

    /// Stop audio playback
    virtual void Stop() = 0;

    /// List available audio devices
    static std::vector<std::string> ListDevices();

    /// Create platform-specific backend
    static std::unique_ptr<AudioBackend> Create();
};

} // namespace AudioCore
