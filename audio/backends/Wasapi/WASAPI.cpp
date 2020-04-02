/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <memory>
#include "WASAPI.hpp"

// ----------------------------------
// Global const uuid of win32 api
// ----------------------------------

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

// ----------------------------------
// WASAPI ctor, dtor
// ----------------------------------

netero::audio::backend::impl::impl() {
	HRESULT	result;
	// Initialize COM library in the current thread
	result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	test_result(result);

	// Create COM device enumerator object
	result = CoCreateInstance(CLSID_MMDeviceEnumerator,
		nullptr,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		reinterpret_cast<void**>(&_device_enumerator));
	test_result(result);
	_render_device = WASAPI_get_default_device(eRender);
	_capture_device = WASAPI_get_default_device(eCapture);
}

netero::audio::backend::impl::~impl() {
	if (renderingThread) {
		renderingState.store(state::OFF, std::memory_order_release);
		renderingThread->join();
		renderingThread.reset();
	}
	if (capturingThread) {
		capturingState.store(state::OFF, std::memory_order_release);
		capturingThread->join();
		capturingThread.reset();
	}
	CoUninitialize();
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

netero::audio::RtCode	netero::audio::backend::setCaptureErrorCallback(const std::function<void(const std::string&)>& cb) {
	pImpl->_captureErrorHandler = cb;
	return RtCode::OK;
}

const std::vector<netero::audio::device>& netero::audio::backend::getRenderDevices() {
	return pImpl->getRenderDevices();
}

netero::audio::RtCode	netero::audio::backend::setRenderDevice(const device& device) {
	if (pImpl->renderingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	return pImpl->setRenderDevice(device);
}

netero::audio::WaveFormat	netero::audio::backend::getRenderFormat() {
	return pImpl->getRenderFormat();
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

const std::vector<netero::audio::device>& netero::audio::backend::getCaptureDevices() {
	return pImpl->getCaptureDevices();
}

netero::audio::RtCode	netero::audio::backend::setCaptureDevice(const netero::audio::device& device) {
	if (pImpl->capturingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	return pImpl->setCaptureDevice(device);
}

netero::audio::WaveFormat	netero::audio::backend::getCaptureFormat() {
	return pImpl->getCaptureFormat();
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

