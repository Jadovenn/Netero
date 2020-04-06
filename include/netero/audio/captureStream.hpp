/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/engine.hpp>
#include <netero/audio/device.hpp>

namespace netero::audio {
	/**
	 * @interface AudioCaptureStream
	 * @brief For any audio container that capture a stream
	 */
	class CaptureStream {
	public:
		CaptureStream(netero::audio::engine& engine, 
			const netero::audio::device& device);
		CaptureStream(netero::audio::engine& engine);
		~CaptureStream();
		netero::audio::engine& getEngine();

		/**
		 * @pure setFormat
		 * @see netero::audio::engine
		 * @remarks You can retrieve the format in ctor via a netero::audio::engine's methode
		 *			to preallocate internal buffer you may use for rendering.
		 * This methode is called by a parent node while the StreamFormat change
		 * and need to be updated in your container.
		 */
		virtual void onFormatChange(const netero::audio::StreamFormat&) = 0;
		netero::audio::OnStreamChangeSlot	onFormatChangeSlot;

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
		netero::audio::CaptureSlot			captureSlot;

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
	protected:
		netero::audio::engine&	_engine;
		netero::audio::device	_device;
	};
}

