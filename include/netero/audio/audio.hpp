/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <vector>
#include <string>

#include <netero/observer/signal.hpp>

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
		DEVICE_TIMEOUT = 3, /**< Time out while accessing/getting device's ressource. The device is probably busy, try later. */
		ERR_NATIVE = 4, /**< Native error. Issued while native call failed, usually the lasterror msg is set or an deviceError signal is issued. */
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
		unsigned	channels = 0; /**< Number of sample per frames. */
		unsigned	samplingFrequency = 0; /**< Actual sampling frequency of the native audio device. */
		std::vector<float> supportedSamplingRate = {}; /**< Supported sampling frequency of the native audio device. */
	};

	/**
	 * @typedef RenderSignal
	 * @brief Render buffer event signal.
	 * This type is used by the engine to declare a signal emitted by a device
	 * while the buffer is available for writting operation. 
	 */
	using RenderSignal = netero::signal<void(float*, const size_t)>;

	/**
	 * @typedef RenderSlot
	 * @brief Render buffer event slot signature.
	 * This type is used by the client to declare a slot to handle a buffer
	 * render event emitted by a device.
	 */
	using RenderSlot = netero::slot<void(float*, const size_t)>;

	/**
	 * @typedef CaptureSignal
	 * @brief Capture buffer event signal signature.
	 * This type is used by the engine to declare a signal emitted by a device
	 * while the buffer is available for capture.
	 */
	using CaptureSignal = netero::signal<void(const float*, const size_t)>;

	/**
	 * @typedef CaptureSlot
	 * @brief Capture buffer slot signature.
	 * This type is used by the client to declare a slot to handle a buffer
	 * capture event emitted by a device.
	 */
	using CaptureSlot = netero::slot<void(const float*, const size_t)>;

	/**
	 * @typedef OnFormatChangeSignal
	 * @brief Format change event signal signature.
	 * This type is used by the engine to declare a signal emittend
	 * by a device while it stream format change.
	 */
	using OnFormatChangeSignal = netero::signal<void(const StreamFormat&)>;

	/**
	 * @typedef OnFormatChangeSlot
	 * @brief Format change event slot signature.
	 * This type is used by the client to declare a slot to handle the format
	 * change event emitted by a device.
	 */
	using OnFormatChangeSlot = netero::slot<void(const StreamFormat&)>;

	/**
	 * @typedef DeviceErrorSignal
	 * @brief Device error signal signature
	 * This type is used by the engine to declare a signal emitted
	 * while an error occure during a device operation.
	 */
	using DeviceErrorSignal = netero::signal<void(const std::string&)>;

	/**
	 * @typedef DeviceErrorSlot
	 * @brief Device error slot signature.
	 * This type is used by the client to declare a slot to connect
	 * with an error signal from a device.
	 */
	using DeviceErrorSlot = netero::slot<void(const std::string&)>;

	/**
	 * @typedef OnDisconnectedSignal
	 * @brief Device disconnected signal signature
	 * This type is used by the engine to declare a signal emitted
	 * while the related device is disconnected.
	 */
	using OnDisconnectedSignal = netero::signal<void()>;

	/**
	 * @typedef onDisconnectedSlot
	 * @brief Device disconnected slot signature.
	 * This type is used by the client to declare a slot to connect
	 * with an OnDisconnectedSignal from a device.
	 * @info the device is invalide after the end of the scope of the slot
	 */
	using OnDisconnectedSlot = netero::slot<void()>;
}

