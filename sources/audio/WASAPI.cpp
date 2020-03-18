/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <functional>
#include <type_traits>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>

#include <objbase.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <Mmreg.h>
#include <avrt.h>

#include <netero/audio/engine.hpp>

// ----------------------------------
// Globals
// ----------------------------------

static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

// ----------------------------------
// WASAPI dependent implementation
// ----------------------------------

class netero::audio::engine::impl {
private:
	IMMDeviceEnumerator* _d_enumerator = nullptr;
	IMMDevice* _device = nullptr;
	IAudioClient* _audio_client = nullptr;
	IAudioRenderClient* _audio_rendering = nullptr;
	WAVEFORMATEX* _wfx = nullptr;
	WAVEFORMATEXTENSIBLE* _wfx_ext = nullptr;
	HANDLE _event = nullptr;
	HANDLE _task = nullptr;
	REFERENCE_TIME _latency;
	unsigned _frameCount;

	std::function<void(float*, size_t)> _cb;

	enum state {
		OFF,
		STOP,
		RUNNING,
		TIMEOUT,
		ERR,
	};

	std::atomic<state>	_state = state::OFF;
	std::unique_ptr<std::thread>	_thread;

	inline void	test_result(HRESULT result) {
		if (FAILED(result)) {
			WASAPI_cleanup();
			throw std::runtime_error("WASAPI Error: " + result);
		}
	}

	template<class T,
		typename = std::enable_if<std::is_base_of<IUnknown, T>::value>>
	inline void WASAPI_release(T** ptr) {
		if (*ptr) {
			(*ptr)->Release();
			*ptr = nullptr;
		}
	}

	inline void	WASAPI_cleanup() {
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

public:
	void	thread_handle() {
		HRESULT result;
		// fill first buffer using callback
		BYTE* buffer = nullptr;
		result = _audio_rendering->GetBuffer(_frameCount, &buffer);
		if (FAILED(result)) {
			_state.store(state::ERR, std::memory_order_release);
			return;
		}
		_cb(reinterpret_cast<float*>(buffer), _frameCount);
		result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
		if (FAILED(result)) {
			_state.store(state::ERR, std::memory_order_release);
			return;
		}
		DWORD taskIndex = 0;
		_task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
		if (!_task) {
			_state.store(state::ERR, std::memory_order_release);
			return;
		}
		result = _audio_client->Start();
		if (FAILED(result)) {
			_state.store(state::ERR, std::memory_order_release);
			return;
		}

		_state.store(state::RUNNING, std::memory_order_release);
		while (_state.load(std::memory_order_acquire) == state::RUNNING) {
			DWORD ret = WaitForSingleObject(_event, 2000);
			if (ret != WAIT_OBJECT_0) { // Timeout
				_audio_client->Stop();
				_state.store(state::TIMEOUT, std::memory_order_release);
				break;
			}
			result = _audio_rendering->GetBuffer(_frameCount, &buffer);
			if (FAILED(result)) {
				_state.store(state::ERR, std::memory_order_release);
				break;
			}
			_cb(reinterpret_cast<float*>(buffer), _frameCount);
			result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
			if (FAILED(result)) {
				_state.store(state::ERR, std::memory_order_release);
				break;
			}
		}
exit:
		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < _latency / 10000) {
			std::this_thread::yield();
		}
		result = _audio_client->Stop();
		if (FAILED(result)) {
			_state.store(state::ERR, std::memory_order_release);
			return;
		}
		if (_task) {
			AvRevertMmThreadCharacteristics(_task);
			_task = nullptr;
		}
	}

public:

	impl() {
		HRESULT	result;

		// Initialize COM library in the current thread
		result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
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
			_latency * 100,
			_latency,
			_wfx,
			nullptr);
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

		// Get buffer size
		result = _audio_client->GetBufferSize(&_frameCount);
		test_result(result);

		// Get the rendering client
		result = _audio_client->GetService(IID_IAudioRenderClient,
			reinterpret_cast<void**>(&_audio_rendering));
		test_result(result);

	}

	~impl() {
		WASAPI_cleanup();
	}

	void					registerCB(std::function<void(float*, size_t)> cb) {
		_cb = cb;
	}

	netero::audio::RtCode	start() {
		HRESULT result;
		if (!_cb) {
			return ERR_MISSING_CALLBACK;
		}
		// fill first buffer using callback
		BYTE* buffer = nullptr;
		result = _audio_rendering->GetBuffer(_frameCount, &buffer);
		if (FAILED(result)) {
			return ERR_NATIVE;
		}
		_cb(reinterpret_cast<float*>(buffer), _frameCount);
		result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
		if (FAILED(result)) {
			return ERR_NATIVE;
		}
		DWORD taskIndex = 0;
		_task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
		if (!_task) {
			return ERR_NATIVE;
		}
		result = _audio_client->Start();
		if (FAILED(result)) {
			return ERR_NATIVE;
		}
		return OK;
	}

	netero::audio::RtCode	stop() {
		HRESULT result;
		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < _latency / 10000 ) {
			std::this_thread::yield();
		}

		result = _audio_client->Stop();
		if (FAILED(result)) {
			return ERR_NATIVE;
		}
		if (_task) {
			AvRevertMmThreadCharacteristics(_task);
			_task = nullptr;
		}
		return OK;
	}

	netero::audio::RtCode	async_start() {
		if (!_cb) {
			return ERR_MISSING_CALLBACK;
		}
		_thread = std::make_unique<std::thread>(std::bind(&netero::audio::engine::impl::thread_handle, this));
		return OK;
	}

	netero::audio::RtCode	async_stop() {
		_state.store(state::STOP, std::memory_order_release);
		_thread->join();
		_thread.reset();
		_state.store(state::OFF, std::memory_order_release);
		return OK;
	}
};

// ----------------------------------------
// Proxy methode from netero::engine class
// ----------------------------------------

netero::audio::engine::engine()
	:	pImpl {std::make_unique<netero::audio::engine::impl>()}
{}

netero::audio::engine::~engine() = default;

void					netero::audio::engine::registerCB(std::function<void(float*, size_t)> cb) {
	pImpl->registerCB(cb);
}

netero::audio::RtCode	netero::audio::engine::start() {
	return pImpl->async_start();
}

netero::audio::RtCode	netero::audio::engine::stop() {
	return pImpl->async_stop();
}

