// iOS Input Backend using GameController framework
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#ifdef __APPLE__

#pragma once
#include <memory>
#include "input_common/input_backend.h"

#ifdef __OBJC__
#import <GameController/GameController.h>

namespace InputCommon {

class iOSInputBackend : public InputBackend {
private:
    ButtonCallback button_callback;
    AnalogCallback analog_callback;
    NSMutableArray* connected_controllers = nil;

public:
    iOSInputBackend();
    ~iOSInputBackend() override;

    bool Initialize() override;
    void Shutdown() override;
    void PollDevices() override;
    std::vector<Common::ParamPackage> GetInputDevices() const override;
    void SetButtonCallback(ButtonCallback callback) override;
    void SetAnalogCallback(AnalogCallback callback) override;
};

} // namespace InputCommon

#endif // __OBJC__

#endif // __APPLE__
