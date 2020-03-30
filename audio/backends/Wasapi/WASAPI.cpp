/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <memory>
#include "WASAPI.hpp"

// ----------------------------------
// WASAPI ctor, dtor
// ----------------------------------

netero::audio::backend::impl::impl() {
	WASAPI_init();
}

netero::audio::backend::impl::~impl() {
	WASAPI_cleanup();
}

void	netero::audio::backend::impl::registerHandle(const std::function<void(float*, size_t)> &cb) {
	_cb = cb;
}

// ----------------------------------------
// Proxy methode from netero::engine class
// ----------------------------------------

netero::audio::backend& netero::audio::backend::GetInstance() {
	static std::unique_ptr<backend>	audio_engine(new backend());
	return *audio_engine;
}

netero::audio::backend::backend()
	:	pImpl(std::make_unique<netero::audio::backend::impl>())
{}

netero::audio::backend::~backend() = default;

void					netero::audio::backend::registerHandle(const std::function<void(float*, size_t)> &cb) {
	pImpl->registerHandle(cb);
}

netero::audio::WaveFormat	netero::audio::backend::getFormat() {
	return pImpl->getFormat();
}

netero::audio::RtCode	netero::audio::backend::start() {
	return pImpl->async_start();
}

netero::audio::RtCode	netero::audio::backend::stop() {
	return pImpl->async_stop();
}

netero::audio::RtCode	netero::audio::backend::poll() {
	return pImpl->poll();
}

size_t	netero::audio::backend::getBufferSize() {
	return  pImpl->getBufferSize();
}

