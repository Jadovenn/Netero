/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

namespace netero::audio {

class Device {
    public:
    virtual ~Device() = 0;

    enum class RtCode {
        SUCCESS
    };

    virtual RtCode open() = 0;
    virtual RtCode close() = 0;
    virtual bool isValid() = 0;

};

} // namespace netero::audio
