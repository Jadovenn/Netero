/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <memory>
#include "WASAPI.hpp"

// ----------------------------------
// WASAPI ctor, dtor
// ----------------------------------

netero::audio::engine::impl::impl() {
	WASAPI_init();
}

netero::audio::engine::impl::~impl() {
	delete _buffer;
	WASAPI_cleanup();
}

void	netero::audio::engine::impl::registerHandle(const std::function<void(float*, size_t)> &cb) {
	_cb = cb;
}

void	netero::audio::engine::impl::alloc_buffer(unsigned frames) {
	_bufferFrameCount = frames * 10;
	_bufferSize = _bufferFrameCount * sizeof(float);
	_WASAPIBufferSize = frames * sizeof(float);
	_buffer = new (std::nothrow) float[_bufferFrameCount];
	if (!_buffer) {
		WASAPI_cleanup();
		throw std::bad_alloc();
	}
	else {
		std::memset(_buffer, _bufferFrameCount, 0);
	}
}

// ----------------------------------------
// Proxy methode from netero::engine class
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

netero::audio::RtCode	netero::audio::engine::async_start() {
	return pImpl->async_start();
}

netero::audio::RtCode	netero::audio::engine::async_stop() {
	return pImpl->async_stop();
}

size_t	netero::audio::engine::getBufferSize() {
	return  pImpl->getBufferSize();
}

