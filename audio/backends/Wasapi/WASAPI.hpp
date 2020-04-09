/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <exception>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <thread>
#include <atomic>
#include <memory>

#include <objbase.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <Mmreg.h>
#include <avrt.h>
#include <comdef.h>
#include <Functiondiscoverykeys_devpkey.h>

#include <netero/audio/audio.hpp>
#include <netero/audio/backend.hpp>
#include <netero/audio/device.hpp>

extern const CLSID CLSID_MMDeviceEnumerator;
extern const IID IID_IMMDeviceEnumerator;
extern const IID IID_IAudioClient;
extern const IID IID_IAudioRenderClient;
extern const IID IID_IAudioCaptureClient;

enum class DataFlow {
	eRender = EDataFlow::eRender,
	eCapture = EDataFlow::eCapture,
	eAll = EDataFlow::eAll,
};

struct WASAPI_device {
	enum class state {
		OFF,
		STOP,
		RUNNING,
		TIMEOUT,
		ERR,
	};

	IMMDevice*				device = nullptr;
	IAudioClient*			audio_client = nullptr;
	IAudioRenderClient*		render_client = nullptr;
	IAudioCaptureClient*	capture_client = nullptr;
	WAVEFORMATEX*			wfx = nullptr;
	WAVEFORMATEXTENSIBLE*	wfx_ext = nullptr;
	REFERENCE_TIME			latency = 0;
	UINT32					framesCount = 0;
	bool					isLoopBackDevice = false;

	DataFlow							deviceFlow = DataFlow::eAll;
	netero::audio::device				clientDevice;
	std::unique_ptr<std::thread>		renderingThread;
	std::unique_ptr<std::thread>		capturingThread;
	std::atomic<state>					renderingState = state::OFF;
	std::atomic<state>					capturingState = state::OFF;

	template<class T,
		typename = std::enable_if<std::is_base_of<IUnknown, T>::value>>
	inline void release(T** ptr) {
		if (*ptr) {
			(*ptr)->Release();
			*ptr = nullptr;
		}
	}

	~WASAPI_device() {
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
		clientDevice.signals.captureStreamSig.flush();
		clientDevice.signals.renderStreamSig.flush();
		clientDevice.signals.deviceStreamFormatChangeSig.flush();
		clientDevice.signals.deviceErrorSig.flush();
		release<IMMDevice>(&device);
		release<IAudioClient>(&audio_client);
		release<IAudioRenderClient>(&render_client);
		release<IAudioCaptureClient>(&capture_client);
		if (wfx) {
			CoTaskMemFree(wfx);
			wfx = nullptr;
			wfx_ext = nullptr;
		}
	}
};

class netero::audio::backend::impl {
public:

	impl();
	~impl();

	enum state {
		OFF,
		STOP,
		RUNNING,
		TIMEOUT,
		ERR,
	};

	std::string			lastError;

	IMMDeviceEnumerator*			device_enumerator = nullptr;
	std::list<std::shared_ptr<WASAPI_device>>	_renderDevices;
	std::list<std::shared_ptr<WASAPI_device>>	_captureDevices;
	netero::audio::device						nullDevice;

	//std::vector<netero::audio::device> renderDevices = {};
	//std::vector<netero::audio::device> captureDevices = {};

	std::shared_ptr<WASAPI_device>	WASAPI_alloc_device(IMMDevice*, DataFlow);
	PROPVARIANT						WASAPI_get_device_property(IMMDevice*, const PROPERTYKEY);
	LPWSTR							WASAPI_get_device_ID(IMMDevice*);
	const std::string				WASAPI_get_default_device_ID(DataFlow);
	std::shared_ptr<WASAPI_device>	WASAPI_get_device(const audio::device&);

	static std::string wstring_to_string(LPWSTR str);

	void	test_result(HRESULT result) {
		if (FAILED(result)) {
			_com_error	err(result);
			throw std::runtime_error("WASAPI Error: " + std::string(err.ErrorMessage()));
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
};

