/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

netero::audio::WaveFormat	netero::audio::backend::impl::getRenderFormat() {
	HRESULT	result;
	WaveFormat	format{};

	if (!_render_device) {
		return format;
	}

	result = _render_device->audio_client->GetBufferSize(&format.framesCount);
	if (result != S_OK) {
		format.framesCount = 0;
	}
	format.bytesPerFrame = _render_device->wfx->nChannels * (_render_device->wfx->wBitsPerSample / 8);
	format.bytesPerSample = (_render_device->wfx->wBitsPerSample / 8);
	format.channels = _render_device->wfx->nChannels;
	format.samplingFrequency = _render_device->wfx->nSamplesPerSec;
	format.supportedSamplingRate.push_back(_render_device->wfx->nSamplesPerSec);
	return format;
}

netero::audio::WaveFormat	netero::audio::backend::impl::getCaptureFormat() {
	HRESULT	result;
	WaveFormat	format{};

	if (!_capture_device) {
		return format;
	}

	result = _capture_device->audio_client->GetBufferSize(&format.framesCount);
	if (result != S_OK) {
		format.framesCount = 0;
	}
	format.bytesPerFrame = _capture_device->wfx->nChannels * (_capture_device->wfx->wBitsPerSample / 8);
	format.bytesPerSample = (_capture_device->wfx->wBitsPerSample / 8);
	format.channels = _capture_device->wfx->nChannels;
	format.samplingFrequency = _capture_device->wfx->nSamplesPerSec;
	format.supportedSamplingRate.push_back(_capture_device->wfx->nSamplesPerSec);
	return format;
}

