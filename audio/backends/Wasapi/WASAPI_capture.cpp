/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

void    netero::audio::backend::impl::capturingThreadHandle() {
	HRESULT result;
	HANDLE	task = nullptr;
	BYTE*	buffer = nullptr;
	DWORD	taskIndex = 0;
	const	size_t bytePerFrames = _capture_device->wfx->nChannels * (_capture_device->wfx->wBitsPerSample / (size_t)8);
	UINT32	padding = 0;
	UINT32	availableFrames = 0;
	DWORD flags;

	task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!task) {
		renderingState.store(state::ERR, std::memory_order_release);
		task = nullptr;
		lastError = "Could not elevate thread priority.";
		goto exit;
	}

	result = _capture_device->audio_client->Start();
	if (FAILED(result)) {
		capturingState.store(state::ERR, std::memory_order_release);
		_com_error	err(result);
		lastError = "Audio Client: " + std::string(err.ErrorMessage());
		goto exit;
	}

	while (capturingState.load(std::memory_order_acquire) == state::RUNNING) {
		result = _capture_device->capture_client->GetNextPacketSize(&padding);
		if (FAILED(result)) {
			capturingState.store(state::ERR, std::memory_order_release);
			_com_error	err(result);
			lastError = "Capture Client: " + std::string(err.ErrorMessage());
			goto exit;
		}
		if (padding != 0) {
			result = _capture_device->capture_client->GetBuffer(&buffer,
				//&availaleFrames,
				&padding,
				&flags,
				nullptr,
				nullptr);
			if (FAILED(result) || buffer == nullptr) {
				renderingState.store(state::ERR, std::memory_order_release);
				_com_error	err(result);
				lastError = "Capture Client: " + std::string(err.ErrorMessage());
				goto exit;
			}
			capturingCallback(reinterpret_cast<float*>(buffer), padding);
			result = _capture_device->capture_client->ReleaseBuffer(padding);
			if (FAILED(result)) {
				renderingState.store(state::ERR, std::memory_order_release);
				_com_error	err(result);
				lastError = "Capture Client: " + std::string(err.ErrorMessage());
				goto exit;
			}
		}
		else { // going to fast, yield()
			std::this_thread::yield();
		}
	}

	result = _capture_device->audio_client->Stop();
	if (FAILED(result)) {
		capturingState.store(state::ERR, std::memory_order_release);
		_com_error	err(result);
		lastError = "Audio Client: " + std::string(err.ErrorMessage());
		goto exit;
	}
	capturingState.store(state::OFF, std::memory_order_release);
	if (task) {
		AvRevertMmThreadCharacteristics(task);
	}
	return;
exit:
	if (_captureErrorHandler) {
		_captureErrorHandler(lastError);
	}
	if (task) {
		AvRevertMmThreadCharacteristics(task);
	}
}

netero::audio::RtCode   netero::audio::backend::impl::startCapture() {
	if (!_capture_device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (!capturingCallback) { return RtCode::ERR_MISSING_CALLBACK; }
	capturingState.store(state::RUNNING, std::memory_order_release);
	capturingThread = std::make_unique<std::thread>(std::bind(&netero::audio::backend::impl::capturingThreadHandle, this));
    return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::impl::stopCapture() {
	if (!_capture_device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (capturingState.load(std::memory_order_acquire) != state::RUNNING) {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	capturingState.store(state::STOP, std::memory_order_release);
    return RtCode::OK;
}

