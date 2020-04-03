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
	const	size_t bytePerFrames = capture_device->wfx->nChannels * (capture_device->wfx->wBitsPerSample / (size_t)8);
	UINT32	padding = 0;
	UINT32	availableFrames = 0;
	DWORD flags;

	task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!task) {
		renderingState.store(state::ERR, std::memory_order_release);
		task = nullptr;
		lastError = "Could not elevate thread priority.";
		goto exit_on_error;
	}

	result = capture_device->audio_client->Start();
	if (FAILED(result)) {
		capturingState.store(state::ERR, std::memory_order_release);
		_com_error	err(result);
		lastError = "Audio Client: " + std::string(err.ErrorMessage());
		goto exit_on_error;
	}

	while (capturingState.load(std::memory_order_acquire) == state::RUNNING) {
		result = capture_device->capture_client->GetNextPacketSize(&padding);
		if (FAILED(result)) {
			capturingState.store(state::ERR, std::memory_order_release);
			_com_error	err(result);
			lastError = "Capture Client: " + std::string(err.ErrorMessage());
			goto exit_on_error;
		}
		if (padding != 0) {
			result = capture_device->capture_client->GetBuffer(&buffer,
				&padding,
				&flags,
				nullptr,
				nullptr);
			if (FAILED(result) || buffer == nullptr) {
				renderingState.store(state::ERR, std::memory_order_release);
				_com_error	err(result);
				lastError = "Capture Client: " + std::string(err.ErrorMessage());
				goto exit_on_error;
			}
			capturingCallback(reinterpret_cast<float*>(buffer), padding);
			result = capture_device->capture_client->ReleaseBuffer(padding);
			if (FAILED(result)) {
				renderingState.store(state::ERR, std::memory_order_release);
				_com_error	err(result);
				lastError = "Capture Client: " + std::string(err.ErrorMessage());
				goto exit_on_error;
			}
		}
		else { // going to fast, yield()
			std::this_thread::yield();
		}
	}

	result = capture_device->audio_client->Stop();
	if (FAILED(result)) {
		capturingState.store(state::ERR, std::memory_order_release);
		_com_error	err(result);
		lastError = "Audio Client: " + std::string(err.ErrorMessage());
		goto exit_on_error;
	}
	capturingState.store(state::OFF, std::memory_order_release);
	if (task) { AvRevertMmThreadCharacteristics(task); }
	return;
exit_on_error:
	if (captureErrorHandler) { captureErrorHandler(lastError); }
	if (task) { AvRevertMmThreadCharacteristics(task); }
}

netero::audio::RtCode   netero::audio::backend::startCapture() {
	if (!pImpl->capture_device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (!pImpl->capturingCallback) { return RtCode::ERR_MISSING_CALLBACK; }
	if (pImpl->capturingState.load(std::memory_order_acquire) == impl::state::RUNNING) {
		return RtCode::ERR_ALREADY_RUNNING;
	}
	pImpl->capturingState.store(impl::state::RUNNING, std::memory_order_release);
	pImpl->capturingThread = std::make_unique<std::thread>(&netero::audio::backend::impl::capturingThreadHandle, pImpl.get());
    return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::stopCapture() {
	if (!pImpl->capture_device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (pImpl->capturingState.load(std::memory_order_acquire) != impl::state::RUNNING) {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	pImpl->capturingState.store(impl::state::STOP, std::memory_order_release);
    return RtCode::OK;
}

