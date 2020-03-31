/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

void    netero::audio::backend::impl::WASAPI_cleanup() {
	if (_wfx) {
		CoTaskMemFree(_wfx);
		_wfx = nullptr;
		_wfx_ext = nullptr;
	}
	WASAPI_release<IMMDeviceEnumerator>(&_device_enumerator);
	WASAPI_release<IMMDevice>(&_render_device);
	WASAPI_release<IMMDevice>(&_capture_device);
	WASAPI_release<IAudioClient>(&_render_client);
	WASAPI_release<IAudioClient>(&_capture_client);
	WASAPI_release<IAudioRenderClient>(&_audio_rendering);
	CoUninitialize();
}

void    netero::audio::backend::impl::WASAPI_init() {
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

	// Get the default rendering audio device
	result = _device_enumerator->GetDefaultAudioEndpoint(eRender,
		eConsole,
		&_render_device);
	test_result(result);

	// Get the default capturing audio device
	result = _device_enumerator->GetDefaultAudioEndpoint(
		eCapture, eConsole, &_capture_device);
	test_result(result);

	// Activate the audio device and get the audio client
	result = _render_device->Activate(IID_IAudioClient,
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&_render_client));
	test_result(result);

	// Retrieve Mix Format
	result = _render_client->GetMixFormat(&_wfx);
	test_result(result);
	if (_wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		_wfx_ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(_wfx);
	}

	// Get device latency
	result = _render_client->GetDevicePeriod(nullptr, &_latency);
	test_result(result);

	// Initialize Audio Client
	result = _render_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
		0,
		0,
		0,
		_wfx,
		nullptr);
	test_result(result);

	// Get buffer size
	result = _render_client->GetBufferSize(&_frameCount);
	test_result(result);

	// Get the rendering client
	result = _render_client->GetService(IID_IAudioRenderClient,
		reinterpret_cast<void**>(&_audio_rendering));
	test_result(result);
}

netero::audio::WaveFormat	netero::audio::backend::impl::getOutputFormat() {
	HRESULT	result;
	WaveFormat	format{};

	if (!_render_client) {
		return format;
	}

	result = _render_client->GetBufferSize(&format.framesCount);
	if (result != S_OK) {
		format.framesCount = 0;
	}
	format.bytesPerFrame = _wfx->nChannels * (_wfx->wBitsPerSample / 8);
	format.bytesPerSample = (_wfx->wBitsPerSample / 8);
	format.channels = _wfx->nChannels;
	format.samplingFrequency = _wfx->nSamplesPerSec;
	format.supportedSamplingRate.push_back(_wfx->nSamplesPerSec);
	return format;
}

netero::audio::WaveFormat	netero::audio::backend::impl::getInputFormat() {
	return WaveFormat{};
}

