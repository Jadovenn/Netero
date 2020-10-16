/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Audio/Device.hpp>

#include <CoreAudio/CoreAudio.h>

static constexpr unsigned SamplingRates[] = { 4000,  5512,  8000,  9600,  11025, 16000,  22050,
                                              32000, 44100, 48000, 88200, 96000, 176400, 192000 };

class DeviceImpl final: public Netero::Audio::Device {
    public:
    explicit DeviceImpl(UInt32 id, AudioObjectPropertyScope scope);
    ~DeviceImpl() noexcept final;

    bool operator==(UInt32 deviceUID) const noexcept { return this->_id == deviceUID; }

    RtCode Open() final;
    RtCode Close() final;

    [[nodiscard]] bool               IsValid() const final { return this->_isValid; }
    [[nodiscard]] bool               IsLoopback() const final { return false; }
    [[nodiscard]] const std::string& GetName() const final { return this->_name; }
    [[nodiscard]] const std::string& GetManufacturer() const final { return this->_manufacturer; };

    [[nodiscard]] const Netero::Audio::Format& GetFormat() const final { return this->_format; }

    void SetProcessingCallback(ProcessingCallbackHandle& cb) final
    {
        this->_processingCallback = cb;
    }

    void ResetProcessingCallback() final { this->_processingCallback = nullptr; }

    void SetAcquisitionCallback(AcquisitionCallbackHandle& cb) final
    {
        this->_acquisitionCallback = cb;
    }

    void ResetAcquisitionCallback() final { this->_acquisitionCallback = nullptr; }

    void SetErrorCallback(ErrorCallbackHandle& cb) final { this->_errorCallback = cb; }

    void ResetErrorCallback() final { this->_errorCallback = nullptr; }

    static OSStatus NativeCallbackHandler(AudioDeviceID inDevice,
                                          const AudioTimeStamp*,
                                          const AudioBufferList* inInputData,
                                          const AudioTimeStamp*,
                                          AudioBufferList* outOutputData,
                                          const AudioTimeStamp*,
                                          void* context);

    void ReportError(const std::string& anErrorMessage)
    {
        if (this->_errorCallback) {
            this->_errorCallback(anErrorMessage);
        }
    }

    private:
    bool                     _isValid;
    bool                     _isOpen;
    UInt32                   _id;
    AudioObjectPropertyScope _scope;
    UInt32                   _bufferIdx;
    AudioDeviceIOProcID      _callbackID;

    Netero::Audio::Format     _format;
    ProcessingCallbackHandle  _processingCallback;
    AcquisitionCallbackHandle _acquisitionCallback;
    ErrorCallbackHandle       _errorCallback;

    std::string _name;
    std::string _manufacturer;
};
