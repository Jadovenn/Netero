/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

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
	_engine.async_start();
}

netero::audio::device::~device() {
	_engine.async_stop();
}

netero::audio::device& netero::audio::device::GetAudioDevice() {
    static std::unique_ptr<device>  audio_device(new device);
    return *audio_device;
}

void    netero::audio::device::handle(float* buffer, size_t size) {
	int	idx = 0;
	int buffer_idx = 0;
	float current = 0;

	while (idx < size) {
		current = static_cast<float>(render(idx, 0));
		buffer[buffer_idx] = current;
		buffer_idx += 1;
		current = static_cast<float>(render(idx, 1));
		buffer[buffer_idx] = current;
		buffer_idx += 1;
		idx += 1;
	}
}

