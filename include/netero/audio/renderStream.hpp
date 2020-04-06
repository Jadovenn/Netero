/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/engine.hpp>
#include <netero/audio/device.hpp>

namespace netero::audio {
	/**
	 * @interface RenderStream
	 * @brief For any audio stream capable of rendering an audio signal.
	 */
	class RenderStream {
	public:
		RenderStream(netero::audio::engine& engine, const netero::audio::device& device);
		RenderStream(netero::audio::engine& engine);
		~RenderStream();
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
		netero::audio::OnStreamChangeSlot	onStreamChangeSlot;

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
		virtual void renderStream(float* buffer, const size_t frames) = 0;
		netero::audio::RenderSlot	renderSlot;

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

	protected:
		netero::audio::engine&	_engine;
		netero::audio::device	_device;
	};
}

