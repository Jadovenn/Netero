/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/audio.hpp>
#include <netero/observer/signals.hpp>

namespace netero::audio {

    /**
     * @struct device
     * @brief Hold information of all available Capture and Render devices.
     */
    struct device {
		std::string		id; /**< Device Id. Exact same stringify id given by the audio backend. */
		std::string		name; /**< Device Name. Device name given by the audio backend */
		std::string		manufacturer; /**< Device manufacturer name. May be "Unknown" if not provided by the audio backend*/
		StreamFormat	format; /**< Device stream format. */

        struct events {
			netero::signals<void(float*, const size_t)>         renderStreamSig;
			netero::signals<void(const float*, const size_t)>   captureStreamSig;
			netero::signals<void(const StreamFormat&)>          deviceStreamForamtChangeSig;
			netero::signals<void(const std::string&)>           deviceErrorSig;
        };
        events  signals;

        bool    operator==(const device& other) {
            return this->id == other.id && this->name == other.name;
        }

        device() = default;

        device(const device& dev) {
            this->id = dev.id;
            this->name = dev.name;
            this->manufacturer = dev.manufacturer;
            this->format = dev.format;
        }

        operator bool() {
            return !this->id.empty();
        }

        const device &operator=(const device &dev) {
            this->id = dev.id;
            this->name = dev.name;
            this->manufacturer = dev.manufacturer;
            this->format = dev.format;
            return *this;
        }

    };
}

