/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <CoreAudio/CoreAudio.h>
#include <netero/audio/engine.hpp>

class netero::audio::engine::impl {
public:
	void registerHandle(const std::function<void(float*, size_t)> &cb);
	WaveFormat getFormat();
	RtCode stop();
	RtCode start();
	RtCode async_start();
	RtCode async_stop();
	RtCode poll();
	size_t getBufferSize();
private:
	void CORE_AUDIO_init();
};

