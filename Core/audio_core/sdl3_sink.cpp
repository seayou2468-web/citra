// Audio Output Sink - Platform-independent implementation
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#include <string>
#include <vector>
#include "audio_core/audio_backend.h"
#include "audio_core/sdl3_sink.h"
#include "common/assert.h"
#include "common/logging/log.h"

namespace AudioCore {

struct SDL3Sink::Impl {
    std::unique_ptr<AudioBackend> backend;
    unsigned int sample_rate = 0;
    std::function<void(s16*, std::size_t)> cb;
};

SDL3Sink::SDL3Sink(std::string device_name) : impl(std::make_unique<Impl>()) {
    impl->backend = AudioBackend::Create();
    if (!impl->backend) {
        LOG_CRITICAL(Audio_Sink, "Failed to create audio backend");
        return;
    }

    if (!impl->backend->Initialize(device_name)) {
        LOG_CRITICAL(Audio_Sink, "Failed to initialize audio backend");
        impl->backend.reset();
        return;
    }

    impl->sample_rate = impl->backend->GetNativeSampleRate();
    if (!impl->backend->Start()) {
        LOG_CRITICAL(Audio_Sink, "Failed to start audio playback");
        impl->backend.reset();
        return;
    }
}

SDL3Sink::~SDL3Sink() {
    if (impl && impl->backend) {
        impl->backend->Stop();
    }
}

unsigned int SDL3Sink::GetNativeSampleRate() const {
    if (!impl || !impl->backend)
        return native_sample_rate;

    return impl->sample_rate;
}

void SDL3Sink::SetCallback(std::function<void(s16*, std::size_t)> cb) {
    if (impl) {
        impl->cb = cb;
        if (impl->backend) {
            impl->backend->SetCallback(cb);
        }
    }
}

std::vector<std::string> ListSDL3SinkDevices() {
    return AudioBackend::ListDevices();
}

} // namespace AudioCore
