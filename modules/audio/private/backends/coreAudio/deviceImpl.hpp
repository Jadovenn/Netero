/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/audio/device.hpp>

#include <CoreAudio/CoreAudio.h>

class DeviceImpl final: public netero::audio::Device {
    public:
    explicit DeviceImpl(UInt32 id) : _id(id) {}
    ~DeviceImpl() final {}

    RtCode open() override { return RtCode::SUCCESS; }
    RtCode close() override { return RtCode::SUCCESS; }
    bool   isValid() override { return false; }

    private:
    UInt32  _id;
};
