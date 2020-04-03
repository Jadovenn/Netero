/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <functional>
#include <vector>
#include <string>

/**
 * @file audio.hpp
 * @brief Default audio declarations
 */

namespace netero::audio {

	/**
	 * @typedef MessageCallback
	 * @brief Signature for message signal.
	 * This is the callback signature for message signal transmission
	 * from the engine to high level container to notify some events and/or errors.
	 */
	using MessageCallback = std::function<void(const std::string&)>;

	/**
	 * @typedef RenderCallback
	 * @brief 
	 */
	using RenderCallback = std::function<void(float*, const size_t)>;
	using CaptureCallback = std::function<void(const float*, const size_t)>;

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

	/**
	 * @struct device
	 * @brief Hold information of all available Capture and Render devices.
	 */
	struct device {
		std::string id; /**< Device Id. Exact same stringify id given by the audio backend. */
		std::string name; /**< Device Name. Device name given by the audio backend */
		std::string manufacturer; /**< Device manufacturer name. May be "Unknown" if not provided by the audio backend*/
	};

	/**
	 * @interface AudioCaptureStream
	 * @brief For any audio stream capable of rendering an audio signal.
	 */
	class AudioRenderStream {
	public:
		/**
		 * @pure setFormat
		 * @see netero::audio::engine
		 * @remarks You can retrieve the format in ctor via a netero::audio::engine's methode
		 *			to preallocate internal buffer you may use for rendering.
		 * This methode is called by a parent node while the StreamFormat change
		 * and need to be updated in your container.
		 */
		virtual void onFormatChange(const StreamFormat &) = 0;

		/**
		 * @pure renderStream
		 * @warning This methode is called by a parent node in a seperate thread.
		 *			You must not perform any allocation nor blocking operation or it might
		 *			impact severly the audio rendering performence of your application.
		 * @warning Becarfull, the buffer size may differ between call. Do not assume
		 *			that it is the same as the shared size provided threw StreamFormat struct.
		 * @param[in] buffer The rendering buffer to write samples to.
		 * @param[in] frames The number of frames the buffer contain.
		 */
		virtual void renderStream(float *buffer, size_t frames) = 0;

		/**
		 * @pure play
		 * @see pause
		 * Allow the audio stream to produce a signal.
		 * In case of previous call of stop the stream should start at
		 * a previously saved offset.
		 */
		virtual void play() = 0;

		/**
		 * @pure pause
		 * Stop the signal generation from the stream and save the offset.
		 * If play() is called again the stream must start again from the saved offset.
		 */
		virtual void pause() = 0;

		/**
		 * @pure stop
		 * Stop the signal generation from the stream.
		 * The offset may not be saved and be reseted.
		 */
		virtual void stop() = 0;
	};

	/**
	 * @interface AudioCaptureStream
	 * @brief For any audio container that capture a stream
	 */
	class AudioCaptureStream {
	public:
		/**
		 * @pure captureStream
		 * @warning This methode is called by a parent node in a seperate thread.
		 *			You must not perform any allocation nor blocking call or it might
		 *			impact severly the audio rendering of your application.
		 * @warning Becarfull, the buffer size may differ between call. Do not assume
					that it is the same as the shared size provided threw WaveFormat struct.
		 * @param[in] buffer The rendering buffer.
		 * @param[in] frames The number of frames the buffer contain.
		 */
		virtual void captureStream(const float* buffer, const size_t frames) = 0;

		/**
		 * @pure record
		 * Allow the audio stream to recive a signal.
		 */
		virtual void record() = 0;

		/**
		 * @pure stop
		 * Stop reciving a signal from the stream.
		 */
		virtual void stop() = 0;
	};
}

