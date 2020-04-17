/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file renderStream.hpp
 * @brief Header containing render interface declaration.
 */

#include <netero/audio/audioStream.hpp>

namespace netero::audio {

	/**
	 * @brief For any audio stream capable of rendering an audio signal.
	 */
	class RenderStream: public AudioStream {
	public:
		RenderStream() = default;
		RenderStream(const RenderStream&) = delete;
		RenderStream(RenderStream&&) noexcept = delete;
		RenderStream& operator=(const RenderStream&) = delete;
		RenderStream& operator=(RenderStream&&) = delete;
		virtual ~RenderStream() = default;

		/**
		 * @warning This method is called by a parent node in a separate thread.
		 *			You must not perform any allocation nor blocking operation or it might
		 *			impact severely the audio rendering performance of your application.
		 * @warning Be careful, the buffer size may differ between call. Do not assume
		 *			that it is the same as the shared size provided threw StreamFormat struct.
		 * @param[in] buffer The rendering buffer to write samples to.
		 * @param[in] frames The number of frames the buffer contain.
		 */
		virtual void renderStream(float* buffer, const size_t frames) = 0;

		/**
		 * @see pause
		 * Allow the audio stream to produce a signal.
		 * In case of previous call of stop the stream should start at
		 * a previously saved offset.
		 */
		virtual void play() = 0;

		/**
		 * Stop the signal generation from the stream and save the offset.
		 * If play() is called again the stream must start again from the saved offset.
		 */
		virtual void pause() = 0;

		/**
		 * Stop the signal generation from the stream.
		 * The offset may not be reset.
		 */
		virtual void stop() = 0;

	};
}

