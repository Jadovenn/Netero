/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/signals.hpp>

#include <iostream>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <Mmreg.h>

// written with C language logic

// ----------------------------------
// Globals
// ----------------------------------

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

// ----------------------------------
// Helpers
// ----------------------------------

// dummy audio interface (my garbage struct)
struct AudioInterface {
	IMMDeviceEnumerator		*device_enumerator = nullptr;
	IMMDevice				*device = nullptr;
	IAudioClient			*audio_client = nullptr;
	IAudioRenderClient		*render_client = nullptr;
	WAVEFORMATEX			*wfx = nullptr;
	BYTE					*buffer = nullptr;
	unsigned				buffer_frame_count = 0;
	REFERENCE_TIME			buffer_duration = 0;
	DWORD					flags = 0;
	WAVEFORMATEXTENSIBLE	*wfx_extended = nullptr;
};

// check pointer, if exist call release and set it to null
template<class T>
inline void	WASAPI_ptr_release(T** ptr) {
	if (*ptr) {
		(*ptr)->Release();
		*ptr = nullptr;
	}
}

// ----------------------------------
// WASAPI Related function
// ----------------------------------

// could be defined as destructor of my garbage struct (but it would be call by ctor)
void	release_audio_interface(AudioInterface* audio_interface) {
	if (!audio_interface)
		return;
	if (audio_interface->wfx) {
		CoTaskMemFree(audio_interface->wfx);
		audio_interface->wfx = nullptr;
	}
	audio_interface->buffer = nullptr;
	audio_interface->buffer_duration = 0;
	audio_interface->buffer_frame_count = 0;
	audio_interface->flags = 0;
	WASAPI_ptr_release<IMMDeviceEnumerator>(&audio_interface->device_enumerator);
	WASAPI_ptr_release<IMMDevice>(&audio_interface->device);
	WASAPI_ptr_release<IAudioClient>(&audio_interface->audio_client);
	WASAPI_ptr_release<IAudioRenderClient>(&audio_interface->render_client);
}

// could be defined as constructor of my carbadge struct
int	init_audio_interface(AudioInterface *audio_interface) {
	HRESULT			result;

	if (!audio_interface)
		return 1;
	// need for CoCreatInstance
	result = CoInitialize(nullptr);
	if (FAILED(result)) {
		goto error_handler;
	}

	// Retreive device enumerator
	result = CoCreateInstance(CLSID_MMDeviceEnumerator,
		nullptr,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		reinterpret_cast<void**>(&audio_interface->device_enumerator));
	if (FAILED(result)) {
		goto error_handler;
	}

	// get audio device
	result = audio_interface->device_enumerator->GetDefaultAudioEndpoint(eRender,
		eConsole,
		&audio_interface->device);
	if (FAILED(result)) {
		goto error_handler;
	}

	// get audio client
	result = audio_interface->device->Activate(IID_IAudioClient,
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&audio_interface->audio_client));
	if (FAILED(result)) {
		goto error_handler;
	}

	// retrieves the stream format of the audio engine
	result = audio_interface->audio_client->GetMixFormat(&audio_interface->wfx);
	if (FAILED(result)) {
		goto error_handler;
	}

	// initialize audio client in shared mode
	result = audio_interface->audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
		0,
		10000000,
		0,
		audio_interface->wfx,
		nullptr);
	if (FAILED(result)) {
		goto error_handler;
	}

	// get allocated buffer size
	result = audio_interface->audio_client->GetBufferSize(&audio_interface->buffer_frame_count);
	if (FAILED(result)) {
		goto error_handler;
	}
	else {
		std::cout << "Buffer size: " << audio_interface->buffer_frame_count << std::endl;
	}

	// get the buffer interface
	result = audio_interface->audio_client->GetService(IID_IAudioRenderClient,
		reinterpret_cast<void**>(&audio_interface->render_client));
	if (FAILED(result)) {
		goto error_handler;
	}

	// compute buffer duration, j'ai la flemme de cast ici, c'est crade je verrais plus tard mon calcule me parait faux...
	audio_interface->buffer_duration = (double)10000000 * audio_interface->buffer_frame_count / audio_interface->wfx->nSamplesPerSec;
	std::cout << "Buffer duration: " << audio_interface->buffer_duration / 10000000 << " second" << std::endl;

	return 0;
error_handler:
	release_audio_interface(audio_interface);
	return 1;
}

int	audio_start(AudioInterface *audio_interface) {
	if (!audio_interface)
		return 1;
	if (audio_interface->audio_client) {
		HRESULT result = audio_interface->audio_client->Start();
		if (FAILED(result)) {
			return 1;
		}
		return 0;
	}
	return 1;
}

int	audio_stop(AudioInterface *audio_interface) {
	if (!audio_interface)
		return 1;
	if (audio_interface->audio_client) {
		HRESULT result = audio_interface->audio_client->Stop();
		if (FAILED(result)) {
			return 1;
		}
		return 0;
	}
	return 1;
}

BYTE* audio_get_buffer(AudioInterface *audio_interface) {
	if (!audio_interface)
		return nullptr;
	BYTE* buffer = nullptr;
	if (audio_interface->render_client) {
		HRESULT result = audio_interface->render_client->GetBuffer(audio_interface->buffer_frame_count, &buffer);
		if (FAILED(result)) {
			return nullptr;
		}
	}
	return buffer;
}

void	audio_release_buffer(AudioInterface* audio_interface) {
	if (!audio_interface)
		return;
	if (audio_interface->render_client) {
		HRESULT result = audio_interface->render_client->ReleaseBuffer(audio_interface->buffer_frame_count, audio_interface->flags);
		if (FAILED(result)) {
			return; // need better error handling
		}
	}
}

// ----------------------------------
// Sinusoid program function and class
// ----------------------------------

int	main() {
	AudioInterface	audio_interface;
	if (init_audio_interface(&audio_interface))
		return 1;
	WAVEFORMATEXTENSIBLE* head = (WAVEFORMATEXTENSIBLE*)audio_interface.wfx;

	// amplitude de 1, a 440 Hz avec une frequence de reference de 48kH et une phase de 0
	netero::signals::Sinusoidal<double>	a_minor { 0.5, 440, 48000, 0};
	// values will be in [-1,1]

	BYTE *byte_buffer = audio_get_buffer(&audio_interface);

	if (byte_buffer) { // fill with E signal here
		float* buffer = reinterpret_cast<float*>(byte_buffer);
		int max = audio_interface.buffer_frame_count;
		int idx = 0;
		int buffer_idx = 0;

		while (idx < max) {
			float current = a_minor(idx);
			buffer[buffer_idx] = current;
			buffer_idx += 1;
			buffer[buffer_idx] = current;
			buffer_idx += 1;
			idx += 1;
		}
	}
	else {
		release_audio_interface(&audio_interface);
		return 1;
	}
	audio_release_buffer(&audio_interface);
	audio_start(&audio_interface);
	Sleep(3000);
	audio_stop(&audio_interface);
	release_audio_interface(&audio_interface);
	return 0;
}
