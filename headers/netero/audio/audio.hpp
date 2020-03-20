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

	struct WaveFormat {
		unsigned	channels;
		unsigned	nBlocks;
		unsigned	samplePerSecond;
	};

	class AudioStream {
	public:
		virtual void setFormat(WaveFormat &) = 0;
		virtual double render(int delta, int channel) = 0;

		virtual void	play() = 0;
		virtual void	pause() = 0;
		virtual void	stop() = 0;
	};
}

