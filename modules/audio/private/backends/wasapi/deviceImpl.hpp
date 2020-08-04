/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <atomic>
#include <future>

#include <netero/audio/device.hpp>

#include "wasapiHelpers.hpp"

#include <AudioClient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <mmdeviceapi.h>

class DeviceImpl final: public netero::audio::Device {
    public:
    explicit DeviceImpl(IMMDevice*, EDataFlow);
    ~DeviceImpl() final;

    bool operator==(IMMDevice* deviceIdentifier)
    {
        LPWSTR myWideStringId = nullptr;
        this->_deviceIdentifier->GetId(&myWideStringId);
        LPWSTR otherWideStringId = nullptr;
        deviceIdentifier->GetId(&otherWideStringId);
        if (wcscmp(myWideStringId, otherWideStringId) == 0) {
            return true;
        }
        return false;
    }

    RtCode open() final;
    RtCode close() final;

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
        this->_processingCallback = cb;
    }

    void resetProcessingCallback() final { this->_processingCallback = nullptr; }

    void setAcquisitionCallback(netero::audio::Device::AcquisitionCallbackHandle& cb) final
    {
        this->_acquisitionCallback = cb;
    }

    void resetAcquisitionCallback() final { this->_acquisitionCallback = nullptr; }

    void setErrorCallback(netero::audio::Device::ErrorCallbackHandle& cb) final
    {
        this->_errorCallback = cb;
    }

    void resetErrorCallback() final { this->_errorCallback = nullptr; }

    private:
    void repportError(const std::string& anErrorMessage)
    {
        if (this->_errorCallback) {
            this->_errorCallback(anErrorMessage);
        }
    }

    private:
    enum class AsyncState { STOP = 0, RUN = 1, SYSTEM_ERROR = 2 };

    std::atomic<AsyncState>                    _renderingAsyncState;
    std::future<netero::audio::Device::RtCode> _renderingFuture;
    static netero::audio::Device::RtCode       RenderingNativeCallback(DeviceImpl*);
    netero::audio::Device::RtCode              openForRendering();
    netero::audio::Device::RtCode              closeAfterRendering();

    std::atomic<AsyncState>                    _acquisitionAsyncState;
    std::future<netero::audio::Device::RtCode> _acquisitionFuture;
    static netero::audio::Device::RtCode       AcquisitionNativeCallback(DeviceImpl*);
    netero::audio::Device::RtCode              openForAcquisition();
    netero::audio::Device::RtCode              closeAfterAcquisition();

    bool          _isValid;
    bool          _isOpen;
    IMMDevice*    _deviceIdentifier;
    EDataFlow     _dataflow;
    IAudioClient* _audioClient;
    WAVEFORMATEX* _wfx;

    netero::audio::Format                            _deviceFomat;
    netero::audio::Device::ProcessingCallbackHandle  _processingCallback;
    netero::audio::Device::AcquisitionCallbackHandle _acquisitionCallback;
    netero::audio::Device::ErrorCallbackHandle       _errorCallback;

    std::string _name;
    std::string _manufacturer;
};
