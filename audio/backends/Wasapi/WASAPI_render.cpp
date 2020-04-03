/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <thread>
#include <chrono>
#include "WASAPI.hpp"

void   netero::audio::backend::impl::renderingThreadHandle() {
	HRESULT	result;
	HANDLE	task = nullptr;
	BYTE*	buffer = nullptr;
	DWORD	taskIndex = 0;
	const size_t bytePerFrames = render_device->wfx->nChannels * (render_device->wfx->wBitsPerSample / (size_t)8);
	UINT32	padding = 0;
	size_t	availaleFrames = 0;

	task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!task) {
		task = nullptr;
		lastError = "Could not elevate thread priority.";
		renderingState.store(state::ERR, std::memory_order_release);
		goto exit_on_error;
	}
	result = render_device->audio_client->Start();
	if (FAILED(result)) {
		renderingState.store(state::ERR, std::memory_order_release);
		lastError = "Could not start the device.";
		goto exit_on_error;
	}
	while (renderingState.load(std::memory_order_acquire) == state::RUNNING) {
		result = render_device->audio_client->GetCurrentPadding(&padding);
		if (FAILED(result)) {
			renderingState.store(state::ERR, std::memory_order_release);
			lastError = "Could not retrieve buffer padding";
			goto exit_on_error;
		}
		else if (padding != render_device->framesCount) { // process signal
			availaleFrames = render_device->framesCount - padding;
			result = render_device->render_client->GetBuffer(availaleFrames, &buffer);
			if (FAILED(result) || buffer == nullptr) {
				renderingState.store(state::ERR, std::memory_order_release);
				lastError = "Could not retrieve device buffer.";
				goto exit_on_error;
			}
			std::memset(buffer, 0, availaleFrames * bytePerFrames);
			renderingCallback(reinterpret_cast<float*>(buffer), availaleFrames);
			result = render_device->render_client->ReleaseBuffer(availaleFrames, 0);
			if (FAILED(result)) {
				renderingState.store(state::ERR, std::memory_order_release);
				lastError = "Could not release device buffer, device stuck.";
				goto exit_on_error;
			}
		}
		else { // going to fast, yield()
			std::this_thread::yield();
		}
	}
	result = render_device->audio_client->Stop();
	if (FAILED(result)) {
		renderingState.store(state::ERR, std::memory_order_release);
		lastError = "Could not stop, device stuck.";
		goto exit_on_error;
	}
	renderingState.store(state::OFF, std::memory_order_release);
	if (task) { AvRevertMmThreadCharacteristics(task); }
	return;
exit_on_error:
	if (renderErrorHandler) { renderErrorHandler(lastError); }
	if (task) { AvRevertMmThreadCharacteristics(task); }
}

netero::audio::RtCode   netero::audio::backend::startRender() {
	BYTE*	buffer = nullptr;
	HRESULT result;

	if (!pImpl->render_device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (!pImpl->renderingCallback) { return RtCode::ERR_MISSING_CALLBACK; }
	if (pImpl->renderingState.load(std::memory_order_acquire) == impl::state::RUNNING) {
		return RtCode::ERR_ALREADY_RUNNING;
	}
	result = pImpl->render_device->render_client->GetBuffer(pImpl->render_device->framesCount, &buffer);
	if (FAILED(result)) {
		pImpl->lastError = "Could not retrieve rendering buffer while start.";
		return RtCode::ERR_NATIVE;
	}
	pImpl->renderingCallback(reinterpret_cast<float*>(buffer), pImpl->render_device->framesCount);
	result = pImpl->render_device->render_client->ReleaseBuffer(pImpl->render_device->framesCount, 0);
	if (FAILED(result)) {
		pImpl->lastError = "Could not release rendering buffer, device stuck.";
		return RtCode::ERR_NATIVE;
	}
	pImpl->renderingState.store(impl::state::RUNNING, std::memory_order_release);
	pImpl->renderingThread = std::make_unique<std::thread>(&netero::audio::backend::impl::renderingThreadHandle, pImpl.get());

	// TODO: wait one sampleFrequency cycle to catch init thread error

	return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::stopRender() {
	if (!pImpl->render_device) {
		return RtCode::ERR_NO_SUCH_DEVICE;
	}
	if (pImpl->renderingState.load(std::memory_order_acquire) != impl::state::RUNNING) {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() < pImpl->render_device->latency) {
		std::this_thread::yield();
	}
	pImpl->renderingState.store(impl::state::STOP, std::memory_order_release);
	start = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() < 500) {
		if (pImpl->renderingState.load(std::memory_order_acquire) == impl::state::OFF) {
			break;
		}
		end = std::chrono::system_clock::now();
	}
	if (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() > 500) {
		pImpl->renderingThread->detach();
		pImpl->renderingThread.reset();
		return RtCode::DEVICE_TIMEOUT;
	}
	pImpl->renderingThread->join();
	pImpl->renderingThread.reset();
	return RtCode::OK;
}

