/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/os.hpp>
#include "WASAPI.hpp"

// ----------------------------------------
// Global const uuid of win32's classes api
// ----------------------------------------

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioSessionControl = __uuidof(IAudioSessionControl);

// ----------------------------------
// Backend::impl ctor, dtor
// ----------------------------------

netero::audio::backend::impl::impl() {

	netero::os::acquireSystemResources();
	
	// Initialize COM library in the current thread
	//result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//test_result(result);

	// Create COM device enumerator object
	HRESULT result = CoCreateInstance(CLSID_MMDeviceEnumerator,
		nullptr,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		reinterpret_cast<void**>(&device_enumerator));
	test_result(result);
}

netero::audio::backend::impl::~impl() {
	netero::os::releaseSystemResources();
	for (auto device: this->_renderDevices) {
		device->Release();
		device.reset();
	}
	for (auto device: this->_captureDevices) {
		device->Release();
		device.reset();
	}
	for (auto device: this->_garbadgeDevices) {
		device->Release();
		device.reset();
	}
	//CoUninitialize();
}

// -------------------------------------------
// netero::audio::backend necessary definition
// -------------------------------------------

netero::audio::backend& netero::audio::backend::GetInstance() {
	static std::unique_ptr<backend>	windowsBackend(new backend);
	return *windowsBackend;
}

netero::audio::backend::backend()
	:	pImpl(std::make_unique<netero::audio::backend::impl>())
{}

netero::audio::backend::~backend() = default;

const std::string& netero::audio::backend::getLastError() const {
	return pImpl->lastError;
}

void netero::audio::backend::setDeviceDisconnectedCallback(const std::function<void(const netero::audio::device&)>& callback) const {
	pImpl->deviceDisconectedCallback = callback;
}

