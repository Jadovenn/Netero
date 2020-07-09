/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <memory>
#include <thread>
#include <chrono>
#include "CoreAudio.hpp"

netero::audio::engine::impl::impl()
	:	_device()
{
	{
		// Necessary init call to make CoreAudio working properly
		CFRunLoopRef loop = nullptr;
		AudioObjectPropertyAddress property = {
				kAudioHardwarePropertyRunLoop,
				kAudioObjectPropertyScopeGlobal,
				kAudioObjectPropertyElementMaster
		};
		OSStatus result = AudioObjectSetPropertyData(kAudioObjectSystemObject,
													 &property,
													 0,
													 nullptr,
													 sizeof(CFRunLoopRef),
													 &loop);
		if (result != noErr) {
			return;
		}
	}
	CORE_AUDIO_get_default_output_device(_device);
}

netero::audio::engine::impl::~impl() {
	if (_device.buffer.callBackId) {
		std::chrono::time_point	start = std::chrono::system_clock::now();
		while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() < _device.format.samplingFrequency) {
			std::this_thread::yield();
		}
		stop();
	}
}

void netero::audio::engine::impl::registerHandle(const std::function<void(float *, size_t)> &cb) {
	_cb = cb;
}

// ----------------------------------------
// Proxy method from netero::engine class
// ----------------------------------------

netero::audio::engine& netero::audio::engine::GetInstance() {
	static std::unique_ptr<engine>	audio_engine(new engine());
	return *audio_engine;
}

netero::audio::engine::engine()
		:	pImpl(std::make_unique<netero::audio::engine::impl>())
{}

netero::audio::engine::~engine() = default;

void					netero::audio::engine::registerHandle(const std::function<void(float*, size_t)> &cb) {
	pImpl->registerHandle(cb);
}

netero::audio::WaveFormat	netero::audio::engine::getFormat() {
	return pImpl->getFormat();
}

netero::audio::RtCode	netero::audio::engine::start() {
	return pImpl->start();
}

netero::audio::RtCode	netero::audio::engine::stop() {
	return pImpl->stop();
}

netero::audio::RtCode	netero::audio::engine::poll() {
	return pImpl->poll();
}

size_t	netero::audio::engine::getBufferSize() {
	return  pImpl->getBufferSize();
}