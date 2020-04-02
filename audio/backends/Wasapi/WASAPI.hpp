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

extern const CLSID CLSID_MMDeviceEnumerator;
extern const IID IID_IMMDeviceEnumerator;
extern const IID IID_IAudioClient;
extern const IID IID_IAudioRenderClient;
extern const IID IID_IAudioCaptureClient;

struct WASAPI_device {
	IMMDevice*				device = nullptr;
	IAudioClient*			audio_client = nullptr;
	IAudioRenderClient*		render_client = nullptr;
	IAudioCaptureClient*	capture_client = nullptr;
	WAVEFORMATEX*			wfx = nullptr;
	WAVEFORMATEXTENSIBLE*	wfx_ext = nullptr;
	REFERENCE_TIME			latency = 0;
	UINT32					framesCount = 0;
	bool					isLoopBackDevice = false;

	template<class T,
		typename = std::enable_if<std::is_base_of<IUnknown, T>::value>>
	inline void release(T** ptr) {
		if (*ptr) {
			(*ptr)->Release();
			*ptr = nullptr;
		}
	}

	~WASAPI_device() {
		release<IMMDevice>(&device);
		release<IAudioClient>(&audio_client);
		release<IAudioRenderClient>(&render_client);
		if (wfx) {
			CoTaskMemFree(wfx);
			wfx = nullptr;
			wfx_ext = nullptr;
		}
	}
};

class netero::audio::backend::impl {
public:
	enum state {
		OFF,
		STOP,
		RUNNING,
		TIMEOUT,
		ERR,
	};

	std::string			lastError;
	RenderCallback		renderingCallback;
	CaptureCallback		capturingCallback;
	std::atomic<state>	renderingState = state::OFF;
	std::atomic<state>	capturingState = state::OFF;
	std::unique_ptr<std::thread>	renderingThread;
	std::unique_ptr<std::thread>	capturingThread;

	std::function<void(const std::string&)>	_captureErrorHandler;
private:
	IMMDeviceEnumerator* _device_enumerator = nullptr;
	std::unique_ptr<WASAPI_device> _render_device;
	std::unique_ptr<WASAPI_device> _capture_device;

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

	std::unique_ptr<WASAPI_device>	WASAPI_get_default_device(EDataFlow dataFlow);
	std::unique_ptr<WASAPI_device>	WASAPI_init_device(EDataFlow flow, IMMDevice*, bool isLoopback = false);
	IMMDevice*						WASAPI_get_device(EDataFlow flow, const netero::audio::device& device);
	RtCode							WASAPI_get_struct_Device(IMMDevice*, device&);

	std::vector<netero::audio::device> _inDevices;
	std::vector<netero::audio::device> _outDevices;
public:

	impl();
	~impl();

	const std::vector<device>   &getRenderDevices();
	RtCode						setRenderDevice(const device&);
	WaveFormat					getRenderFormat();
	void						renderingThreadHandle();
	RtCode						startRender();
	RtCode						stopRender();

	const std::vector<device>   &getCaptureDevices();
	RtCode						setCaptureDevice(const device&);
	WaveFormat					getCaptureFormat();
	void						capturingThreadHandle();
	RtCode						startCapture();
	RtCode						stopCapture();

	const std::string&			getLastError();
};

