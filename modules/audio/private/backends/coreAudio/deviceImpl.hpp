/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/audio/device.hpp>

#include <CoreAudio/CoreAudio.h>

static constexpr unsigned SamplingRates[] = { 4000,  5512,  8000,  9600,  11025, 16000,  22050,
                                              32000, 44100, 48000, 88200, 96000, 176400, 192000 };

class DeviceImpl final: public netero::audio::Device {
    public:
    explicit DeviceImpl(UInt32 id, AudioObjectPropertyScope scope);
    ~DeviceImpl() noexcept final;

    bool operator==(UInt32 deviceUID) const noexcept { return this->_id == deviceUID; }

    RtCode open() final;
    RtCode close() final;

    [[nodiscard]] bool               isValid() const final { return this->_isValid; }
    [[nodiscard]] bool               isLoopback() const final { return false; }
    [[nodiscard]] const std::string& getName() const final { return this->_name; }
    [[nodiscard]] const std::string& getManufacturer() const final { return this->_manufacturer; };

    [[nodiscard]] const netero::audio::Format& getFormat() const final { return this->_format; }

    void setProcessingCallback(ProcessingCallbackHandle& cb) final {
        this->_processingCallback = cb;
    }

    void resetProcessingCallback() final {
        this->_processingCallback = nullptr;
    }

    void setAcquisitionCallback(AcquisitionCallbackHandle& cb) final {
        this->_acquisitionCallback = cb;
    }

    void resetAcquisitionCallback() final {
        this->_acquisitionCallback = nullptr;
    }

    void setErrorCallback(ErrorCallbackHandle& cb) final {
        this->_errorCallback = cb;
    }

    void resetErrorCallback() final {
        this->_errorCallback = nullptr;
    }

    static OSStatus NativeCallbackHandler(AudioDeviceID inDevice,
                                          const AudioTimeStamp*,
                                          const AudioBufferList* inInputData,
                                          const AudioTimeStamp*,
                                          AudioBufferList* outOutputData,
                                          const AudioTimeStamp*,
                                          void* context);

    void reportError(const std::string& anErrorMessage) {
        if (this->_errorCallback) {
            this->_errorCallback(anErrorMessage);
        }
    }

    private:
    bool                     _isValid;
    UInt32                   _id;
    AudioObjectPropertyScope _scope;

    netero::audio::Format     _format;
    ProcessingCallbackHandle  _processingCallback;
    AcquisitionCallbackHandle _acquisitionCallback;
    ErrorCallbackHandle       _errorCallback;

    std::string _name;
    std::string _manufacturer;
};
