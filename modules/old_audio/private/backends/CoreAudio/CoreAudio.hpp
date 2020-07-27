/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <CoreAudio/CoreAudio.h>
#include <netero/audio/engine.hpp>

struct Buffer {
	AudioDeviceIOProc callBackId;
	volatile int id;
	std::function<void(float*, size_t)> callBack;
	Buffer() : callBackId(nullptr), id(-1)
	{}

	void reset() {
		callBackId = nullptr;
		id = -1;
	}
};

struct Device {
	AudioDeviceID deviceId;
	netero::audio::WaveFormat format;
	Buffer	buffer;
	Device(): deviceId(0), format(), buffer()
	{}
};

static constexpr unsigned SamplingRates[] = {
		4000, 5512, 8000, 9600, 11025, 16000, 22050,
		32000, 44100, 48000, 88200, 96000, 176400, 192000
};

class netero::audio::engine::impl {
public:
	impl();
	~impl();
	void registerHandle(const std::function<void(float*, size_t)> &cb);
	WaveFormat getFormat();
	RtCode stop();
	RtCode start();
	RtCode poll();
	size_t getBufferSize();
private:
	std::function<void(float*, size_t)> _cb;
	Device _device;
	AudioDeviceIOProcID _ioProcId = nullptr;
	static OSStatus callbackHandler( AudioDeviceID inDevice,
									 const AudioTimeStamp*,
									 const AudioBufferList*,
									 const AudioTimeStamp*,
									 AudioBufferList* outOutputData,
									 const AudioTimeStamp*,
									 void* infoPointer );
	static unsigned CORE_AUDIO_get_device_count();
	static RtCode CORE_AUDIO_get_default_output_device(Device &);
};
