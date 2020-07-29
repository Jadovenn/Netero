/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/audio/device.hpp>

#include <AudioClient.h>
#include <mmdeviceapi.h>

class DeviceImpl final: public netero::audio::Device {
    public:
    explicit DeviceImpl(IMMDevice*, EDataFlow);
    ~DeviceImpl() final;

    RtCode open() final { return RtCode::SUCCESS; }
    RtCode close() final { return RtCode::SUCCESS; }

    [[nodiscard]] bool isValid() const final { return this->_isValid; }
    [[nodiscard]] bool isLoopback() const final { return _dataflow == EDataFlow::eAll; }
    [[nodiscard]] const std::string& getName() const final { return this->_name; }
    [[nodiscard]] const std::string& getManufacturer() const final { return this->_manufacturer; };

    [[nodiscard]] const netero::audio::Format& getFormat() const final
    {
        return this->_deviceFomat;
    }

    void setProcessingCallback(netero::audio::Device::ProcessingCallbackHandle& cb) final
    {
        this->_userCallback = cb;
    }

    void setErrorCallback(netero::audio::Device::ErrorCallbackHandle& cb) final
    {
        this->_userErrorCallback = cb;
    }

    private:
    void repportError(const std::string& anErrorMessage)
    {
        if (this->_userErrorCallback) {
            this->_userErrorCallback(anErrorMessage);
        }
    }

    private:
    bool          _isValid;
    IMMDevice*    _deviceInterface;
    EDataFlow     _dataflow;
    IAudioClient* _audioClient;
    WAVEFORMATEX* _wfx;

    netero::audio::Format                           _deviceFomat;
    netero::audio::Device::ProcessingCallbackHandle _userCallback;
    netero::audio::Device::ErrorCallbackHandle      _userErrorCallback;

    std::string _name;
    std::string _manufacturer;
};
