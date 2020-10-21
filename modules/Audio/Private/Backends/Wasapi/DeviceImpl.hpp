/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <atomic>
#include <future>

#include <netero/audio/device.hpp>

#include "WasapiHelpers.hpp"

#include <AudioClient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <mmdeviceapi.h>

class DeviceImpl final: public Netero::Audio::Device {
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

    RtCode Open() final;
    RtCode Close() final;

    [[nodiscard]] bool IsValid() const final { return this->_isValid; }
    [[nodiscard]] bool IsLoopback() const final { return _dataflow == EDataFlow::eAll; }
    [[nodiscard]] const std::string& GetName() const final { return this->_name; }
    [[nodiscard]] const std::string& GetManufacturer() const final { return this->_manufacturer; };

    [[nodiscard]] const Netero::Audio::Format& GetFormat() const final
    {
        return this->_deviceFomat;
    }

    void SetProcessingCallback(Netero::Audio::Device::ProcessingCallbackHandle& cb) final
    {
        this->_processingCallback = cb;
    }

    void ResetProcessingCallback() final { this->_processingCallback = nullptr; }

    void SetAcquisitionCallback(Netero::Audio::Device::AcquisitionCallbackHandle& cb) final
    {
        this->_acquisitionCallback = cb;
    }

    void ResetAcquisitionCallback() final { this->_acquisitionCallback = nullptr; }

    void SetErrorCallback(Netero::Audio::Device::ErrorCallbackHandle& cb) final
    {
        this->_errorCallback = cb;
    }

    void ResetErrorCallback() final { this->_errorCallback = nullptr; }

    private:
    void RepportError(const std::string& anErrorMessage)
    {
        if (this->_errorCallback) {
            this->_errorCallback(anErrorMessage);
        }
    }

    private:
    enum class AsyncState { STOP = 0, RUN = 1, SYSTEM_ERROR = 2 };

    std::atomic<AsyncState>                    _renderingAsyncState;
    std::future<Netero::Audio::Device::RtCode> _renderingFuture;
    static Netero::Audio::Device::RtCode       RenderingNativeCallback(DeviceImpl*);
    Netero::Audio::Device::RtCode              openForRendering();
    Netero::Audio::Device::RtCode              closeAfterRendering();

    std::atomic<AsyncState>                    _acquisitionAsyncState;
    std::future<Netero::Audio::Device::RtCode> _acquisitionFuture;
    static Netero::Audio::Device::RtCode       AcquisitionNativeCallback(DeviceImpl*);
    Netero::Audio::Device::RtCode              openForAcquisition();
    Netero::Audio::Device::RtCode              closeAfterAcquisition();

    bool          _isValid;
    bool          _isOpen;
    IMMDevice*    _deviceIdentifier;
    EDataFlow     _dataflow;
    IAudioClient* _audioClient;
    WAVEFORMATEX* _wfx;

    Netero::Audio::Format                            _deviceFomat;
    Netero::Audio::Device::ProcessingCallbackHandle  _processingCallback;
    Netero::Audio::Device::AcquisitionCallbackHandle _acquisitionCallback;
    Netero::Audio::Device::ErrorCallbackHandle       _errorCallback;

    std::string _name;
    std::string _manufacturer;
};
