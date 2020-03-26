/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/engine.hpp>

class netero::audio::engine::impl {
public:
	impl();
	~impl();
	void registerHandle(const std::function<void(float*, size_t)> &cb);
	WaveFormat getFormat();
	RtCode stop();
	RtCode start();
	RtCode async_start();
	RtCode async_stop();
	RtCode poll();
	size_t getBufferSize();
private:
	bool _is_device_open;
	unsigned _device_id;
	static constexpr unsigned _samplingRates[] = {
			4000, 5512, 8000, 9600, 11025, 16000, 22050,
			32000, 44100, 48000, 88200, 96000, 176400, 192000
	};

	void CORE_AUDIO_init();
	void CORE_AUDIO_release();
	unsigned CORE_AUDIO_get_device_count();
	unsigned CORE_AUDIO_get_default_output_device();

	std::function<void(float*, size_t)> _cb;
};
