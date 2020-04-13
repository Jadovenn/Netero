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
extern const IID IID_IAudioSessionControl;

std::string wstring_to_string(LPCWSTR str);

enum class DataFlow {
	eRender = EDataFlow::eRender,
	eCapture = EDataFlow::eCapture,
	eAll = EDataFlow::eAll,
};

struct WASAPI_device: public IAudioSessionEvents {
	~WASAPI_device();
	enum class state {
		OFF,
		STOP,
		RUNNING,
		TIMEOUT,
		ERR,
	};

	// IAudioSessionEvent impl related
	LONG	_cRef = 1;
	ULONG STDMETHODCALLTYPE		AddRef() final;
	ULONG STDMETHODCALLTYPE		Release() final;
	HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID, void**) final;
	HRESULT STDMETHODCALLTYPE	OnDisplayNameChanged(LPCWSTR, LPCGUID) final;
	HRESULT STDMETHODCALLTYPE	OnIconPathChanged(LPCWSTR, LPCGUID) final;
	HRESULT STDMETHODCALLTYPE	OnSimpleVolumeChanged(float, BOOL, LPCGUID) final;
	HRESULT STDMETHODCALLTYPE	OnChannelVolumeChanged(DWORD, float*, DWORD, LPCGUID) final;
	HRESULT STDMETHODCALLTYPE	OnGroupingParamChanged(LPCGUID, LPCGUID) final;
	HRESULT STDMETHODCALLTYPE	OnStateChanged(AudioSessionState) final;
	HRESULT STDMETHODCALLTYPE	OnSessionDisconnected(AudioSessionDisconnectReason) final;
	std::function<void(const netero::audio::device&)>		backendDisconnectCallback;

	// Wasapi device management
	IMMDevice*				device = nullptr;
	IAudioClient*			audio_client = nullptr;
	IAudioSessionControl*	audioSession = nullptr;
	IAudioRenderClient*		render_client = nullptr;
	IAudioCaptureClient*	capture_client = nullptr;
	WAVEFORMATEX*			wfx = nullptr;
	WAVEFORMATEXTENSIBLE*	wfx_ext = nullptr;
	REFERENCE_TIME			latency = 0;
	UINT32					framesCount = 0;
	bool					isLoopBackDevice = false;
	std::atomic<bool>		isSessionActive = true;

	// Rendering thread related
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
	std::list<std::shared_ptr<WASAPI_device>>	_garbadgeDevices;
	netero::audio::device						nullDevice;
	std::function<void(const netero::audio::device&)> deviceDisconectedCallback;

	std::shared_ptr<WASAPI_device>	WASAPI_alloc_device(IMMDevice*, DataFlow);
	void							WASAPI_refresh_devices_list(DataFlow);
	PROPVARIANT						WASAPI_get_device_property(IMMDevice*, const PROPERTYKEY);
	LPWSTR							WASAPI_get_device_ID(IMMDevice*);
	const std::string				WASAPI_get_default_device_ID(DataFlow);
	std::shared_ptr<WASAPI_device>	WASAPI_get_device(const audio::device&);

	void							WASAPI_disconnect_device(const audio::device&);


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

