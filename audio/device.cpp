/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <chrono>
#include <functional>
#include <memory>
#include <netero/audio/device.hpp>

netero::audio::device::device()
    :  _engine(netero::audio::engine::GetInstance())
{
	_engine.registerHandle(std::bind(&netero::audio::device::handle,
		this,
		std::placeholders::_1,
		std::placeholders::_2));
	setFormat(_engine.getFormat());
	_engine.start();
}

netero::audio::WaveFormat& netero::audio::device::getWaveFormat() {
	return _format;
}

netero::audio::device::~device() {
	_streams.clear();
	stop();
	_engine.stop();
}

netero::audio::device& netero::audio::device::GetAudioDevice() {
    static std::unique_ptr<device>  audio_device(new device);
    return *audio_device;
}

void    netero::audio::device::handle(float* buffer, size_t size) {
	//std::chrono::time_point	start = std::chrono::system_clock::now();
	//std::cout << size << std::endl;
	std::memset(buffer, 0, size * _format.bytesPerFrame);
	render(buffer, size);
	//std::cout << "duration of the loop: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() << std::endl;
}

