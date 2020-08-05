/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <netero/audio/format.hpp>

namespace netero::audio {

class Device {
    public:
    virtual ~Device() = 0;

    enum class RtCode {
        SUCCESS,
        NO_REGISTERED_CALLBACK,
        SYSTEM_ERROR,
        TIME_OUT,
        ALREADY_OPEN,
        NOT_OPEN
    };

    using ProcessingCallbackHandle = std::function<void(float*, unsigned)>;
    using AcquisitionCallbackHandle = std::function<void(const float*, unsigned)>;
    using ErrorCallbackHandle = std::function<void(const std::string&)>;

    virtual RtCode open() = 0;
    virtual RtCode close() = 0;
    virtual void   setProcessingCallback(ProcessingCallbackHandle&) = 0;
    virtual void   resetProcessingCallback() = 0;
    virtual void   setAcquisitionCallback(AcquisitionCallbackHandle&) = 0;
    virtual void   resetAcquisitionCallback() = 0;
    virtual void   setErrorCallback(ErrorCallbackHandle&) = 0;
    virtual void   resetErrorCallback() = 0;

    [[nodiscard]] virtual bool               isValid() const = 0;
    [[nodiscard]] virtual bool               isLoopback() const = 0;
    [[nodiscard]] virtual const std::string& getName() const = 0;
    [[nodiscard]] virtual const std::string& getManufacturer() const = 0;
    [[nodiscard]] virtual const Format&      getFormat() const = 0;
};

} // namespace netero::audio
