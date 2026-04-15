// SDL Compatibility Layer - Provides backward compatibility
// Maps SDL interface to new platform-independent backends
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "common/param_package.h"

namespace InputCommon::SDL {

// Forward declaration
class State;

class State {
public:
    State();
    ~State();

    void PollDevices();
    std::vector<Common::ParamPackage> GetInputDevices();

private:
    class Impl;
    std::unique_ptr<Impl> impl;

    friend std::unique_ptr<State> Init();
};

std::unique_ptr<State> Init();

} // namespace InputCommon::SDL
