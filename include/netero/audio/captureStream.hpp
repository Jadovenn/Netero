/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file captureStream.hpp
 * @brief Header containing capture interface declaration.
 */

#include <netero/audio/audioStream.hpp>

namespace netero::audio {
	/**
	 * @brief Parent interface for any capture container.
	 */
	class CaptureStream: public AudioStream {
	public:
		CaptureStream() = default;
		CaptureStream(const CaptureStream&) = delete;
		CaptureStream(CaptureStream&&) = delete;
		CaptureStream& operator=(const CaptureStream&) = delete;
		CaptureStream& operator=(CaptureStream&&) = delete;
		virtual ~CaptureStream() = default;

		/**
		 * @brief audio capture callback
		 * @warning This method is called by a parent node in a separate thread.
		 *			You must not perform any allocation nor blocking call or it might
		 *			impact severely the audio rendering of your application.
		 * @warning Be careful, the buffer size may differ between call. Do not assume
					that it is the same as the shared size provided threw WaveFormat struct.
		 * @param[in] buffer The rendering buffer.
		 * @param[in] frames The number of frames the buffer contain.
		 */
		virtual void captureStream(const float* buffer, const size_t frames) = 0;

		/**
		 * Ask the audio stream to start recording of the signal.
		 */
		virtual void record() = 0;

		/**
		 * Ask the audio stream to pause the recording.
		 * But it may restart later.
		 */
		virtual void pause() = 0;
		
		/**
		 * Ask the audio stream to terminate capture.
		 */
		virtual void stop() = 0;
	};
}

