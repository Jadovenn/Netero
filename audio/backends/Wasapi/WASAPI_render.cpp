/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <thread>
#include <chrono>
#include "WASAPI.hpp"

void   netero::audio::backend::impl::renderingThreadHandle() {
	HRESULT result;
	HANDLE	task = nullptr;
	BYTE* buffer = nullptr;
	DWORD taskIndex = 0;
	const size_t bytePerFrames = _wfx->nChannels * (_wfx->wBitsPerSample / (size_t)8);
	UINT32 padding = 0;
	size_t availaleFrames = 0;
	task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!task) {
		task = nullptr;
		lastError = "Could not elevate thread priority.";
		renderingState.store(state::ERR, std::memory_order_release);
		goto exit;
	}
	result = _render_client->Start();
	if (FAILED(result)) {
		renderingState.store(state::ERR, std::memory_order_release);
		lastError = "Could not start the device.";
		goto exit;
	}
	while (renderingState.load(std::memory_order_acquire) == state::RUNNING) {
		result = _render_client->GetCurrentPadding(&padding);
		if (FAILED(result)) {
			renderingState.store(state::ERR, std::memory_order_release);
			lastError = "Could not retrieve buffer padding";
			goto exit;
		}
		if (padding != _frameCount) { // process signal
			availaleFrames = _frameCount - padding;
			result = _audio_rendering->GetBuffer(availaleFrames, &buffer);
			if (FAILED(result) || buffer == nullptr) {
				renderingState.store(state::ERR, std::memory_order_release);
				lastError = "Could not retrieve device buffer.";
				goto exit;
			}
			std::memset(buffer, 0, availaleFrames * bytePerFrames);
			renderingCallback(reinterpret_cast<float*>(buffer), availaleFrames);
			result = _audio_rendering->ReleaseBuffer(availaleFrames, 0);
			if (FAILED(result)) {
				renderingState.store(state::ERR, std::memory_order_release);
				lastError = "Could not release device buffer, device stuck.";
				goto exit;
			}
		}
		else { // going to fast, yield()
			std::this_thread::yield();
		}
	}
	result = _render_client->Stop();
	if (FAILED(result)) {
		renderingState.store(state::ERR, std::memory_order_release);
		lastError = "Could not stop, device stuck.";
		goto exit;
	}
	renderingState.store(state::OFF, std::memory_order_release);
exit:
	if (task) {
		AvRevertMmThreadCharacteristics(task);
	}
}

netero::audio::RtCode   netero::audio::backend::impl::startRender() {
	BYTE* buffer = nullptr;
	HRESULT result;

	if (!renderingCallback) {
		return RtCode::ERR_MISSING_CALLBACK;
	}
	result = _audio_rendering->GetBuffer(_frameCount, &buffer);
	if (FAILED(result)) {
		lastError = "Could not retrieve rendering buffer while start.";
		return RtCode::ERR_NATIVE;
	}
	renderingCallback(reinterpret_cast<float*>(buffer), _frameCount);
	result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
	if (FAILED(result)) {
		lastError = "Could not release rendering buffer, device stuck.";
		return RtCode::ERR_NATIVE;
	}
	renderingState.store(state::RUNNING, std::memory_order_release);
	renderingThread = std::make_unique<std::thread>(std::bind(&netero::audio::backend::impl::renderingThreadHandle, this));

	// wait 500 ns to catch init thread error

	return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::impl::stopRender() {
	if (renderingState.load(std::memory_order_acquire) != state::RUNNING) {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() < _latency) {
		std::this_thread::yield();
	}
	renderingState.store(state::STOP, std::memory_order_release);
	start = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() < 500) {
		if (renderingState.load(std::memory_order_acquire) == state::OFF) {
			break;
		}
		end = std::chrono::system_clock::now();
	}
	if (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() > 500) {
		renderingThread->detach();
		renderingThread.reset();
		return RtCode::DEVICE_TIMEOUT;
	}
	renderingThread->join();
	renderingThread.reset();
	return RtCode::OK;
}

