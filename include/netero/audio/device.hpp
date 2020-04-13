/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/audio.hpp>
#include <netero/observer/signal.hpp>

namespace netero::audio {

    /**
     * @brief A reference container that represent physical audio devices.
     */
    struct device {
        device() = default;
        ~device() = default;
        device(const device& other) {
            this->id = other.id;
            this->name = other.name;
            this->manufacturer = other.manufacturer;
            this->isLoopback = other.isLoopback;
            this->format = other.format;
            this->signals = other.signals;
        }

        device &operator=(const device &other) {
            if (*this == other) {
                return *this;
            }
            this->id = other.id;
            this->name = other.name;
            this->manufacturer = other.manufacturer;
            this->format = other.format;
            this->isLoopback = other.isLoopback;
            this->signals = other.signals;
            return *this;
        }

    	device(device &&other) noexcept {
        	// exception may be raised by string copy operator
        	try {
				*this = other;
                other.id.clear();
                other.name.clear();
                other.manufacturer.clear();
                other.format.clear();
                other.signals.clear();
        	}
        	catch (...) {
                this->id.clear();
                this->name.clear();
                this->manufacturer.clear();
                this->format.clear();
                this->signals.clear();
        	}
        }

    	device& operator=(device &&other) noexcept {
        	// exception may be raised by string copy operator
        	try {
				*this = other;
                other.id.clear();
                other.name.clear();
                other.manufacturer.clear();
                other.format.clear();
                other.signals.clear();
        	}
        	catch (...) {
                this->id.clear();
                this->name.clear();
                this->manufacturer.clear();
                this->format.clear();
                this->signals.clear();
        	}
            return *this;
        }

        bool    operator==(const device& other) const {
            return this->id == other.id
        			&& this->name == other.name
        			&& this->isLoopback == this->isLoopback;
        }
    	
		explicit operator bool() const {
        	return !this->id.empty();
        }
    	
		std::string		id; /**< Device Id. Exact same stringify id given by the audio backend. */
		std::string		name; /**< Device Name. Device name given by the audio backend */
		std::string		manufacturer; /**< Device manufacturer name. May be "Unknown" if not provided by the audio backend*/
        bool            isLoopback = false; /**< This is a loopback device. This flag is set while OS support lp device and it is actually one of them*/
		StreamFormat	format; /**< Device stream format. */

        struct signalsSet {
        	/**
        	 * @brief Set all signal's pointer to nullptr;
        	 */
            void    clear() noexcept {
                renderStreamSig = nullptr;
                captureStreamSig = nullptr;
                deviceStreamFormatChangeSig = nullptr;
                deviceErrorSig = nullptr;
                deviceDisconnectedSig = nullptr;
            }
			netero::signal<void(float*, const size_t)>          *renderStreamSig = nullptr; /**< Signal emitted while device's buffer available for rendering. */
			netero::signal<void(const float*, const size_t)>    *captureStreamSig = nullptr; /**< Signal emitted while device's buffer available for capture. */
			netero::signal<void(const StreamFormat&)>           *deviceStreamFormatChangeSig = nullptr; /**< Signal emitted while the configuration format changed. */
			netero::signal<void(const std::string&)>            *deviceErrorSig = nullptr; /**< Signal emitted while the device encounter an error. */
			netero::signal<void(const std::string&)>            *deviceDisconnectedSig = nullptr; /**< Signal emitted just before the device become unavailable. */
        };
        signalsSet  signals; /**< Set of signal managed by the device. */

    };
}

