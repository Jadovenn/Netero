/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/audio/device.hpp>

#include <CoreAudio/CoreAudio.h>

class DeviceImpl final: public netero::audio::Device {
    public:
    explicit DeviceImpl(UInt32 id);
    ~DeviceImpl() noexcept final;

    RtCode                           open() final { return RtCode::SUCCESS; }
    RtCode                           close() final { return RtCode::SUCCESS; }
    [[nodiscard]] bool               isValid() const final { return this->_isValid; }
    [[nodiscard]] const std::string& getName() const final { return this->_name; }
    [[nodiscard]] const std::string& getManufacturer() const final { return this->_manufacturer; };

    private:
    bool   _isValid;
    UInt32 _id;

    std::string _name;
    std::string _manufacturer;
};
