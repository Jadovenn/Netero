/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

/**
 * @file captureStream.hpp
 * @brief Header containing capture interface declaration.
 */

#include <netero/audio/engine.hpp>
#include <netero/audio/device.hpp>

namespace netero::audio {
	/**
	 * @interface CaptureStream
	 * @brief Parent interface for any capture container.
	 */
	class CaptureStream {
	public:
		/**
		 * @brief Constructor, init engine, connect slot to device.
		 * Initialize the engine, and connect all relevent slot to the given device.
		 */
		CaptureStream(netero::audio::engine& engine, 
			const netero::audio::device& device);

		/**
		 * @brief Constructor, only initialize the engine.
		 * The stream while not capture anything until the client
		 * manually connect to a device.
		 */
		CaptureStream(netero::audio::engine& engine);

		/**
		 * @brief Destructor, disconnect any pending signal connection.
		 */
		~CaptureStream();

		/**
		 * @brief Getter of the audio engine refered by the stream
		 */
		netero::audio::engine& getEngine();

		/**
		 * @see netero::audio::engine
		 * @remarks You can retrieve the format in ctor via a netero::audio::engine's methode
		 *			to preallocate internal buffer you may use for rendering.
		 * This methode is called by a parent node while the StreamFormat change
		 * and need to be updated in your container.
		 */
		virtual void onFormatChange(const netero::audio::StreamFormat&) = 0;

		netero::audio::OnFormatChangeSlot
			onFormatChangeSlot; /**< Slot to be set by the client to be notified in case the device's format change. */

		/**
		 * @brief callback for deviceDisconnected event.
		 * This callback is called by the device while it is disconnected.
		 * The reference from the engine is still valide untile the end of the scope of the slot.
		 */
		virtual void onDisconnected(const std::string &) = 0;
		netero::audio::OnDisconnectedSlot
			onDisconnectedSlot; /**< Slot to be set by the client to be notified while the device is disconnected.  */

		/**
		 * @brief audio capure callback
		 * @warning This methode is called by a parent node in a seperate thread.
		 *			You must not perform any allocation nor blocking call or it might
		 *			impact severly the audio rendering of your application.
		 * @warning Becarfull, the buffer size may differ between call. Do not assume
					that it is the same as the shared size provided threw WaveFormat struct.
		 * @param[in] buffer The rendering buffer.
		 * @param[in] frames The number of frames the buffer contain.
		 */
		virtual void captureStream(const float* buffer, const size_t frames) = 0;
		netero::audio::CaptureSlot
			captureSlot; /**< Slot to be set by the client to be notified in case the buffer is ready to be read. */

		/**
		 * Allow the audio stream to recive a signal.
		 */
		virtual void record() = 0;

		/**
		 * Stop reciving a signal from the stream.
		 */
		virtual void stop() = 0;
	protected:
		netero::audio::engine&	_engine;
		netero::audio::device	_device;
	};
}

