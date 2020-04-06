/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <vector>
#include <string>

#include <netero/observer/signals.hpp>

/**
 * @file audio.hpp
 * @brief Default audio declarations
 */

namespace netero::audio {

	/**
	 * @enum RtCode
	 * @brief Return code for native audio interfaces.
	 */
	enum class RtCode {
		OK = 0, /**< Success. Issued if native call success. */
		ABILITY_NOT_SUPPORTED = 1, /**< Functionality not supported. Issued while the native device does not support the call. */
		FORMAT_NOT_SUPPORTED = 2, /**< The native device does not support the requested format. */
		DEVICE_TIMEOUT = 3, /**< The time out while accessing/getting device's ressource. The device is probably busy, try later. */
		ERR_NATIVE = 4, /**< Native error. Issued while native call failed, usually the lasterror msg is set or a message signal is issued. */
		ERR_MISSING_CALLBACK = 5, /**< Audio callback is missing. You forgot to setup the audio callback. */
		ERR_ALREADY_RUNNING = 6, /**< You already have start the device. */
		ERR_DEVICE_NOT_RUNNING = 7, /**< You try to stop the device but it is not running. */
		ERR_ALTER_RUNNING = 8, /**< You try to change a device or a handler while the device is running. Stop the device before. */
		ERR_NO_SUCH_DEVICE = 9, /**< You reffer to an unknown device. */
	};

	/**
	 * @struct StreamFormat
	 * @brief Hold information relative to the audio native device.
	 * The buffer is a set of Frames, each frames contains samples.
	 * The number of samples per frame is defined by the number of channels.
	 * The size in byte of the buffer is determinate by
	 * the number of bytes per samples time the number of samples.
	 */
	struct StreamFormat {
		unsigned	framesCount = 0; /**< Number of frames in contained in the shared buffer with the device. */
		unsigned	bytesPerFrame = 0; /**< Size in byte for one frame. */
		unsigned	bytesPerSample = 0; /**< Size in byte for one sample. */
		unsigned	channels = 0; /**< Number of sample per frames */
		unsigned	samplingFrequency = 0; /**< Actual sampling frequency of the native audio device. */
		std::vector<float> supportedSamplingRate = {}; /**< Supported sampling frequency of the native audio device */
	};

	using RenderSignal = netero::signals<void(float*, const size_t)>;
	using RenderSlot = netero::slot<void(float*, const size_t)>;
	using CaptureSignal = netero::signals<void(const float*, const size_t)>;
	using CaptureSlot = netero::slot<void(const float*, const size_t)>;
	using OnStreamChangeSignal = netero::signals<void(const StreamFormat&)>;
	using OnStreamChangeSlot = netero::slot<void(const StreamFormat&)>;
	using DeviceErrorSignal = netero::signals<void(const std::string&)>;
	using DeviceErrorSlot = netero::slot<void(const std::string&)>;
}

