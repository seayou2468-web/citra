// Linux Input Backend using /dev/input/event*
// Copyright 2026 Citra Emulator Project
// Licensed under GPLv2 or any later version

#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <map>
#include "input_common/input_backend.h"
#include "common/logging/log.h"

namespace InputCommon {

class LinuxInputBackend : public InputBackend {
private:
    struct DeviceInfo {
        int fd = -1;
        std::string guid;
        std::string name;
    };

    std::map<int, DeviceInfo> devices;
    std::thread poll_thread;
    std::atomic<bool> running{false};

    ButtonCallback button_callback;
    AnalogCallback analog_callback;

    void PollThreadFunc();
    std::string GetDeviceGUID(const std::string& path);
    bool OpenDevice(const std::string& path);

public:
    ~LinuxInputBackend() override {
        Shutdown();
    }

    bool Initialize() override {
        // Scan for input devices
        DIR* dir = opendir("/dev/input");
        if (!dir) {
            LOG_ERROR(Input, "Failed to open /dev/input");
            return false;
        }

        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (strncmp(entry->d_name, "event", 5) == 0) {
                std::string path = "/dev/input/" + std::string(entry->d_name);
                OpenDevice(path);
            }
        }
        closedir(dir);

        running = true;
        poll_thread = std::thread(&LinuxInputBackend::PollThreadFunc, this);
        return true;
    }

    void Shutdown() override {
        running = false;
        if (poll_thread.joinable()) {
            poll_thread.join();
        }
        for (auto& [fd, dev] : devices) {
            if (dev.fd != -1) {
                close(dev.fd);
            }
        }
        devices.clear();
    }

    void PollDevices() override {
        // Polling is done in separate thread
    }

    std::vector<Common::ParamPackage> GetInputDevices() const override {
        std::vector<Common::ParamPackage> result;
        for (const auto& [fd, dev] : devices) {
            Common::ParamPackage package;
            package.Set("engine", "native");
            package.Set("id", dev.guid);
            package.Set("display", dev.name);
            result.push_back(package);
        }
        return result;
    }

    void SetButtonCallback(ButtonCallback callback) override {
        button_callback = callback;
    }

    void SetAnalogCallback(AnalogCallback callback) override {
        analog_callback = callback;
    }
};

bool LinuxInputBackend::OpenDevice(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return false;
    }

    char name[256] = {0};
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);

    DeviceInfo info;
    info.fd = fd;
    info.name = name;
    info.guid = path; // Use path as unique identifier

    devices[fd] = info;
    return true;
}

void LinuxInputBackend::PollThreadFunc() {
    struct input_event ev;

    while (running) {
        for (auto& [fd, dev] : devices) {
            if (dev.fd < 0) continue;

            while (read(dev.fd, &ev, sizeof(ev)) == sizeof(ev)) {
                if (ev.type == EV_KEY && button_callback) {
                    // Button press/release
                    button_callback(dev.guid, ev.code);
                } else if (ev.type == EV_ABS && analog_callback) {
                    // Analog stick/trigger: normalize to -1.0 to 1.0
                    float value = (ev.value - 128) / 128.0f;
                    analog_callback(dev.guid, ev.code, value);
                }
            }
        }
        usleep(10000); // 10ms poll interval
    }
}

std::unique_ptr<InputBackend> InputBackend::Create() {
    auto backend = std::make_unique<LinuxInputBackend>();
    if (backend->Initialize()) {
        return backend;
    }
    return nullptr;
}

} // namespace InputCommon
