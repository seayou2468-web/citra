// iOS Audio Backend using AVAudioEngine
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#ifdef __APPLE__

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "audio_core/audio_backend.h"
#include "audio_core/audio_types.h"

#ifdef __OBJC__
#import <AVFoundation/AVFoundation.h>

namespace AudioCore {

class iOSAudioBackend : public AudioBackend {
private:
    AVAudioEngine* audio_engine = nullptr;
    AVAudioPlayerNode* player_node = nullptr;
    unsigned int sample_rate = 0;
    std::function<void(s16*, std::size_t)> callback;

public:
    ~iOSAudioBackend() override;
    
    bool Initialize(const std::string& device_name) override;
    unsigned int GetNativeSampleRate() const override;
    void SetCallback(std::function<void(s16*, std::size_t)> cb) override;
    bool Start() override;
    void Stop() override;
};

} // namespace AudioCore

#endif // __OBJC__

#endif // __APPLE__
