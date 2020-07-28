/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/audio/device.hpp>

#include <mmdeviceapi.h>

class DeviceImpl final: public netero::audio::Device {
    public:
    explicit DeviceImpl(IMMDevice*);
    ~DeviceImpl() final;

    RtCode             open() override { return RtCode::SUCCESS; }
    RtCode             close() override { return RtCode::SUCCESS; }
    bool               isValid() override { return this->_isValid; }
    const std::string& getName() final { return this->_name; }
    const std::string& getManufacturer() final { return this->_manufacturer; };

    private:
    bool       _isValid;
    IMMDevice* _deviceInterface;

    std::string _name;
    std::string _manufacturer;
};
