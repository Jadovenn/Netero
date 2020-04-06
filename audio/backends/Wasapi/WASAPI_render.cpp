/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <thread>
#include <chrono>
#include "WASAPI.hpp"

static
void   renderingThreadHandle(std::weak_ptr<WASAPI_device> device) {
	HRESULT	result;
	HANDLE	task = nullptr;
	BYTE*	buffer = nullptr;
	DWORD	taskIndex = 0;
	UINT32	padding = 0;
	size_t	availaleFrames = 0;

	auto nativeDevice = device.lock();
	if (!nativeDevice) { return; }
	const size_t bytePerFrames = nativeDevice->wfx->nChannels * (nativeDevice->wfx->wBitsPerSample / (size_t)8);
	task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!task) {
		task = nullptr;
		nativeDevice->clientDevice.signals.deviceErrorSig("Could not elevate thread priority.");
		goto exit;
	}
	result = nativeDevice->audio_client->Start();
	if (FAILED(result)) {
		nativeDevice->clientDevice.signals.deviceErrorSig("Could not start the device.");
		goto exit;
	}
	while (nativeDevice->renderingState.load(std::memory_order_acquire) == WASAPI_device::state::RUNNING) {
		nativeDevice.reset();
		nativeDevice = device.lock();
		if (!nativeDevice) { return; }
		result = nativeDevice->audio_client->GetCurrentPadding(&padding);
		if (FAILED(result)) {
			nativeDevice->clientDevice.signals.deviceErrorSig("Could not retrieve buffer padding.");
			goto exit;
		}
		else if (padding != nativeDevice->framesCount) {
			availaleFrames = nativeDevice->framesCount - padding;
			result = nativeDevice->render_client->GetBuffer(availaleFrames, &buffer);
			if (FAILED(result) || buffer == nullptr) {
				nativeDevice->clientDevice.signals.deviceErrorSig("Could not retrieve buffer padding.");
				goto exit;
			}
			std::memset(buffer, 0, availaleFrames * bytePerFrames);
			nativeDevice->clientDevice.signals.renderStreamSig(reinterpret_cast<float*>(buffer), availaleFrames);
			result = nativeDevice->render_client->ReleaseBuffer(availaleFrames, 0);
			if (FAILED(result)) {
				nativeDevice->clientDevice.signals.deviceErrorSig("Could not release buffer.");
				goto exit;
			}
		}
		else {
			std::this_thread::yield();
		}
	}
	result = nativeDevice->audio_client->Stop();
	if (FAILED(result)) {
		nativeDevice->clientDevice.signals.deviceErrorSig("Could not stop, device stuck.");
	}
exit:
	nativeDevice->renderingState.store(WASAPI_device::state::OFF, std::memory_order_release);
	nativeDevice.reset();
	if (task) { AvRevertMmThreadCharacteristics(task); }
}

netero::audio::RtCode   netero::audio::backend::deviceStartRendering(const netero::audio::device &device) {
	BYTE*	buffer = nullptr;
	HRESULT result;

	auto nativeDevice = pImpl->WASAPI_get_device(device);
	if (!nativeDevice) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (nativeDevice->deviceFlow != DataFlow::eRender) { return RtCode::ABILITY_NOT_SUPPORTED; }
	if (nativeDevice->render_client) { return RtCode::ERR_ALREADY_RUNNING; }
	if (nativeDevice->renderingState.load(std::memory_order_acquire) != WASAPI_device::state::OFF) {
		return RtCode::ERR_ALREADY_RUNNING;
	}

	result = nativeDevice->audio_client->GetService(IID_IAudioRenderClient,
		reinterpret_cast<void**>(&nativeDevice->render_client));
	if (FAILED(result)) {
		_com_error	err(result);
		nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
		nativeDevice->reset();
		return RtCode::ERR_NATIVE;
	}
	result = nativeDevice->render_client->GetBuffer(nativeDevice->framesCount, &buffer);
	if (FAILED(result)) {
		_com_error	err(result);
		nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
		nativeDevice->reset();
		return RtCode::ERR_NATIVE;
	}
	std::memset(buffer, 0, nativeDevice->framesCount * nativeDevice->clientDevice.format.bytesPerFrame);
	nativeDevice->clientDevice.signals.renderStreamSig(reinterpret_cast<float*>(buffer), nativeDevice->framesCount);
	result = nativeDevice->render_client->ReleaseBuffer(nativeDevice->framesCount, 0);
	if (FAILED(result)) {
		_com_error	err(result);
		nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
		nativeDevice->reset();
		return RtCode::ERR_NATIVE;
	}

	nativeDevice->renderingState.store(WASAPI_device::state::RUNNING, std::memory_order_release);
	nativeDevice->renderingThread = std::make_unique<std::thread>(std::bind(renderingThreadHandle, nativeDevice));
	return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::deviceStopRendering(const netero::audio::device &device) {
	auto nativeDevice = pImpl->WASAPI_get_device(device);
	if (!nativeDevice) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (nativeDevice->renderingState.load(std::memory_order_acquire) != WASAPI_device::state::RUNNING) {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() < nativeDevice->latency) {
		std::this_thread::yield();
	}
	nativeDevice->renderingState.store(WASAPI_device::state::STOP, std::memory_order_release);
	start = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() < 500) {
		if (nativeDevice->renderingState.load(std::memory_order_acquire) == WASAPI_device::state::OFF) {
			break;
		}
		end = std::chrono::system_clock::now();
	}
	if (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() > 500) {
		nativeDevice->renderingThread->detach();
		nativeDevice->renderingThread.reset();
		return RtCode::DEVICE_TIMEOUT;
	}
	nativeDevice->renderingThread->join();
	nativeDevice->renderingThread.reset();
	nativeDevice.reset();
	return RtCode::OK;
}

