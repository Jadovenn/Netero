/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <memory>
#include "WASAPI.hpp"

// ----------------------------------------
// Global const uuid of win32's classes api
// ----------------------------------------

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
		reinterpret_cast<void**>(&device_enumerator));
	test_result(result);
	render_device = WASAPI_get_default_device(eRender);
	capture_device = WASAPI_get_default_device(eCapture);
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
// netero::audio::backend definition
// ----------------------------------------

netero::audio::backend& netero::audio::backend::GetInstance() {
	static std::unique_ptr<backend>	audio_engine(new backend());
	return *audio_engine;
}

netero::audio::backend::backend()
	:	pImpl(std::make_unique<netero::audio::backend::impl>())
{}

netero::audio::backend::~backend() = default;

const std::string& netero::audio::backend::getLastError() {
	return pImpl->lastError;
}

netero::audio::RtCode	netero::audio::backend::setCaptureErrorCallback(const std::function<void(const std::string&)>& cb) {
	pImpl->captureErrorHandler = cb;
	return RtCode::OK;
}

netero::audio::RtCode	netero::audio::backend::setRenderErrorCallback(const std::function<void(const std::string&)>& cb) {
	pImpl->renderErrorHandler = cb;
	return RtCode::OK;
}

netero::audio::RtCode	netero::audio::backend::setRenderCallback(const RenderCallback& callback) {
	if (pImpl->renderingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	pImpl->renderingCallback = callback;
	return RtCode::OK;
}

netero::audio::RtCode	netero::audio::backend::setCaptureCallback(const CaptureCallback& callback) {
	if (pImpl->capturingState.load(std::memory_order_acquire) != impl::state::OFF) {
		return RtCode::ERR_ALTER_RUNNING;
	}
	pImpl->capturingCallback = callback;
	return RtCode::OK;
}

