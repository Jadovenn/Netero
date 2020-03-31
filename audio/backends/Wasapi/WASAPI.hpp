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

private:
	IMMDeviceEnumerator* _device_enumerator = nullptr;
	IMMDevice* _render_device = nullptr;
	IMMDevice* _capture_device = nullptr;
	IAudioClient* _render_client = nullptr;
	IAudioClient* _capture_client = nullptr;
	IAudioRenderClient* _audio_rendering = nullptr;
	WAVEFORMATEX* _wfx = nullptr;
	WAVEFORMATEXTENSIBLE* _wfx_ext = nullptr;
	REFERENCE_TIME _latency;
	unsigned _frameCount = 0;
	unsigned _bufferFrameCount = 0;
	size_t	_bufferSize = 0;
	size_t	_WASAPIBufferSize = 0;

	void	test_result(HRESULT result) {
		if (FAILED(result)) {
			_com_error	err(result);
			WASAPI_cleanup();
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

	void WASAPI_init();
	void WASAPI_cleanup();
	RtCode	WASAPI_get_struct_Device(IMMDevice*, device&);
	IMMDevice* WASAPI_get_device(EDataFlow flow, const netero::audio::device& device);


	std::vector<netero::audio::device> _inDevices;
	std::vector<netero::audio::device> _outDevices;
public:

	impl();
	~impl();

	const std::vector<device>   &getOutputDevices();
	RtCode						setOutputDevice(const device&);
	WaveFormat					getOutputFormat();
	void						renderingThreadHandle();
	RtCode						startRender();
	RtCode						stopRender();


	const std::vector<device>   &getInputDevices();
	RtCode						setInputDevice(const device&);
	WaveFormat					getInputFormat();
	void						capturingThreadHandle();
	RtCode						startCapture();
	RtCode						stopCapture();

	const std::string&			getLastError();
};

