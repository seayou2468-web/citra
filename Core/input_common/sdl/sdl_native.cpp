// SDL Input Backend - Platform-independent wrapper
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#include <memory>
#include "common/param_package.h"
#include "common/logging/log.h"
#include "input_common/input_backend.h"
#include "input_common/sdl/sdl.h"
#include "input_common/sdl/sdl_impl.h"

namespace InputCommon::SDL {

class State::Impl {
public:
    std::unique_ptr<InputBackend> backend;
};

State::State() : impl(std::make_unique<Impl>()) {
    impl->backend = InputBackend::Create();
    if (!impl->backend) {
        LOG_WARNING(Input, "No native input backend available");
    }
}

State::~State() = default;

std::unique_ptr<State> Init() {
    auto state = std::make_unique<State>();
    if (!state->impl->backend) {
        LOG_WARNING(Input, "Input backend initialization failed");
    }
    return state;
}

void State::PollDevices() {
    if (impl->backend) {
        impl->backend->PollDevices();
    }
}

std::vector<Common::ParamPackage> State::GetInputDevices() {
    if (!impl->backend) {
        return {};
    }
    return impl->backend->GetInputDevices();
}

} // namespace InputCommon::SDL
