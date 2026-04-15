#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "common/param_package.h"

namespace InputCommon {

/**
 * Abstract input backend interface
 * Platform-specific implementations: iOS (GameController), Linux (/dev/input)
 */
class InputBackend {
public:
    virtual ~InputBackend() = default;

    /// Initialize input backend
    virtual bool Initialize() = 0;

    /// Shutdown input backend
    virtual void Shutdown() = 0;

    /// Poll input devices for state changes
    virtual void PollDevices() = 0;

    /// Get list of connected input devices
    virtual std::vector<Common::ParamPackage> GetInputDevices() const = 0;

    /// Button press event callback: (device_guid, button_code)
    using ButtonCallback = std::function<void(const std::string&, u32)>;

    /// Analog stick event callback: (device_guid, axis_id, value -1.0 to 1.0)
    using AnalogCallback = std::function<void(const std::string&, u32, float)>;

    /// Register callbacks for input events
    virtual void SetButtonCallback(ButtonCallback callback) = 0;
    virtual void SetAnalogCallback(AnalogCallback callback) = 0;

    /// Create platform-specific backend
    static std::unique_ptr<InputBackend> Create();
};

} // namespace InputCommon
