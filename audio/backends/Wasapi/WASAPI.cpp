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
	if (renderingThread) {
		renderingThread->join();
		renderingThread.reset();
	}
	WASAPI_cleanup();
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

const std::string& netero::audio::backend::impl::getLastError() {
	return lastError;
}

// ----------------------------------------
// Backend Output methode
// ----------------------------------------

const std::vector<netero::audio::device>& netero::audio::backend::getOutputDevices() {
	return pImpl->getOutputDevices();
}

netero::audio::RtCode	netero::audio::backend::setOutputDevice(const device& device) {
	if (pImpl->renderingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	pImpl->setOutputDevice(device);
	return RtCode::OK;
}

netero::audio::WaveFormat	netero::audio::backend::getOutputFormat() {
	return pImpl->getOutputFormat();
}

netero::audio::RtCode	netero::audio::backend::startRender() {
	if (pImpl->renderingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALREADY_RUNNING;
	}
	return pImpl->startRender();
}

netero::audio::RtCode	netero::audio::backend::stopRender() {
	pImpl->stopRender();
	return RtCode::OK;
}

netero::audio::RtCode	netero::audio::backend::setRenderCallback(const RenderCallback& callback) {
	if (pImpl->renderingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	pImpl->renderingCallback = callback;
	return RtCode::OK;
}

const std::vector<netero::audio::device>& netero::audio::backend::getInputDevices() {
	return pImpl->getInputDevices();
}

netero::audio::RtCode	netero::audio::backend::setInputDevice(const netero::audio::device& device) {
	if (pImpl->capturingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	pImpl->setInputDevice(device);
	return RtCode::OK;
}

netero::audio::WaveFormat	netero::audio::backend::getInputFormat() {
	return pImpl->getInputFormat();
}

netero::audio::RtCode	netero::audio::backend::startCapture() {
	if (pImpl->capturingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	return pImpl->startCapture();
}

netero::audio::RtCode	netero::audio::backend::stopCapture() {
	return pImpl->stopCapture();
}

netero::audio::RtCode	netero::audio::backend::setCaptureCallback(const CaptureCallback& callback) {
	if (pImpl->capturingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	pImpl->capturingCallback = callback;
	return RtCode::OK;
}

