/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <Netero/Audio/Format.hpp>

namespace Netero::Audio {

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

    virtual RtCode Open() = 0;
    virtual RtCode Close() = 0;
    virtual void   SetProcessingCallback(ProcessingCallbackHandle&) = 0;
    virtual void   ResetProcessingCallback() = 0;
    virtual void   SetAcquisitionCallback(AcquisitionCallbackHandle&) = 0;
    virtual void   ResetAcquisitionCallback() = 0;
    virtual void   SetErrorCallback(ErrorCallbackHandle&) = 0;
    virtual void   ResetErrorCallback() = 0;

    [[nodiscard]] virtual bool               IsValid() const = 0;
    [[nodiscard]] virtual bool               IsLoopback() const = 0;
    [[nodiscard]] virtual const std::string& GetName() const = 0;
    [[nodiscard]] virtual const std::string& GetManufacturer() const = 0;
    [[nodiscard]] virtual const Format&      GetFormat() const = 0;
};

} // namespace Netero::Audio
