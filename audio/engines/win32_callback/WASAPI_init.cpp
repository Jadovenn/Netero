/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

void    netero::audio::engine::impl::WASAPI_cleanup() {
	if (_wfx) {
		CoTaskMemFree(_wfx);
		_wfx = nullptr;
		_wfx_ext = nullptr;
	}
	if (_event) {
		CloseHandle(_event);
		_event = nullptr;
	}
	if (_task) {
		AvRevertMmThreadCharacteristics(_task);
		_task = nullptr;
	}
	WASAPI_release<IMMDeviceEnumerator>(&_d_enumerator);
	WASAPI_release<IMMDevice>(&_device);
	WASAPI_release<IAudioClient>(&_audio_client);
	WASAPI_release<IAudioRenderClient>(&_audio_rendering);
	CoUninitialize();
}

void    netero::audio::engine::impl::WASAPI_init() {
	HRESULT	result;

	// Initialize COM library in the current thread
	result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	test_result(result);

	// Create COM device enumerator object
	result = CoCreateInstance(CLSID_MMDeviceEnumerator,
		nullptr,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		reinterpret_cast<void**>(&_d_enumerator));
	test_result(result);

	// Get the default audio device
	result = _d_enumerator->GetDefaultAudioEndpoint(eRender,
		eConsole,
		&_device);
	test_result(result);

	// Activate the audio device and get the audio client
	result = _device->Activate(IID_IAudioClient,
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&_audio_client));
	test_result(result);

	// Retrieve Mix Format
	result = _audio_client->GetMixFormat(&_wfx);
	test_result(result);
	if (_wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		_wfx_ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(_wfx);
	}

	// Get device latency
	result = _audio_client->GetDevicePeriod(nullptr, &_latency);
	test_result(result);

	// Initialize Audio Client
	result = _audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
		_latency * 10,
		0,
		_wfx,
		nullptr);
	test_result(result);

	// Get buffer size
	result = _audio_client->GetBufferSize(&_frameCount);
	test_result(result);

	// Create buffer swap event
	_event = CreateEvent(nullptr, false, false, nullptr);
	if (!_event) {
		WASAPI_cleanup();
		throw std::runtime_error("WASAPI could not init event");
	}
	// Set the event to the audio client
	result = _audio_client->SetEventHandle(_event);
	test_result(result);

	// Get the rendering client
	result = _audio_client->GetService(IID_IAudioRenderClient,
		reinterpret_cast<void**>(&_audio_rendering));
	test_result(result);
}

netero::audio::WaveFormat	netero::audio::engine::impl::getFormat() {
	WaveFormat	format {};

	format.framesCount = getBufferSize();
	format.bytesPerFrame = _wfx->nChannels * (_wfx->wBitsPerSample / 8);
	format.bytesPerSample = (_wfx->wBitsPerSample / 8);
	format.channels = _wfx->nChannels;
	format.samplingFrequency = _wfx->nSamplesPerSec;
	return format;
}

size_t	netero::audio::engine::impl::getBufferSize() {
	HRESULT	result;
	unsigned size;
	result = _audio_client->GetBufferSize(&size);
	return size;
}
