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
	 * framesCount: nb of frame in the shared buffer with the hardware
	 * channels: number of channel
	 * samplePerSecond: nb of sample processed per second
	 */
	struct WaveFormat {
		unsigned	framesCount;
		unsigned	channels;
		unsigned	samplePerSecond;
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

