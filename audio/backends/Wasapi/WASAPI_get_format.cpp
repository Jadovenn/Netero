/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

netero::audio::StreamFormat	netero::audio::backend::getRenderFormat() {
	HRESULT			result;
	StreamFormat	format{};

	if (!pImpl->render_device) {
		return format;
	}

	result = pImpl->render_device->audio_client->GetBufferSize(&format.framesCount);
	if (result != S_OK) {
		format.framesCount = 0;
	}
	format.bytesPerFrame = pImpl->render_device->wfx->nChannels * (pImpl->render_device->wfx->wBitsPerSample / 8);
	format.bytesPerSample = (pImpl->render_device->wfx->wBitsPerSample / 8);
	format.channels = pImpl->render_device->wfx->nChannels;
	format.samplingFrequency = pImpl->render_device->wfx->nSamplesPerSec;
	format.supportedSamplingRate.push_back(pImpl->render_device->wfx->nSamplesPerSec);
	return format;
}

netero::audio::StreamFormat	netero::audio::backend::getCaptureFormat() {
	HRESULT			result;
	StreamFormat	format{};

	if (!pImpl->capture_device) {
		return format;
	}

	result = pImpl->capture_device->audio_client->GetBufferSize(&format.framesCount);
	if (result != S_OK) {
		format.framesCount = 0;
	}
	format.bytesPerFrame = pImpl->capture_device->wfx->nChannels * (pImpl->capture_device->wfx->wBitsPerSample / 8);
	format.bytesPerSample = (pImpl->capture_device->wfx->wBitsPerSample / 8);
	format.channels = pImpl->capture_device->wfx->nChannels;
	format.samplingFrequency = pImpl->capture_device->wfx->nSamplesPerSec;
	format.supportedSamplingRate.push_back(pImpl->capture_device->wfx->nSamplesPerSec);
	return format;
}

