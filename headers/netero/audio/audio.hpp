/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <functional>

namespace netero::audio {

	enum class RtCode {
		OK = 0,
		ERR_MISSING_CALLBACK = 1,
		ERR_NATIVE = 2,
	};

	/**
	 * framesCount: nb of frame of the shared buffer
	 * bytesPerFrame: size in byte of one frame
	 * bytesPerSample: size in byte of one sample
	 * channels: number of samples per channel
	 * samplingFrequency: sampling rate of the native device in Hz
	 */
	struct WaveFormat {
		unsigned	framesCount;
		unsigned	bytesPerFrame;
		unsigned	bytesPerSample;
		unsigned	channels;
		unsigned	samplingFrequency;
	};

	class AudioStream {
	public:
		virtual void setFormat(WaveFormat &) = 0;
		virtual void render(float *buffer, size_t size) = 0;

		virtual void	play() = 0;
		virtual void	pause() = 0;
		virtual void	stop() = 0;
	};
}

