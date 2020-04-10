/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <thread>
#include <chrono>
#include "WASAPI.hpp"

static
void    capturingThreadHandle(std::weak_ptr<WASAPI_device> device) {
	HRESULT result;
	HANDLE	task = nullptr;
	BYTE*	buffer = nullptr;
	DWORD	taskIndex = 0;
	UINT32	padding = 0;
	UINT32	availableFrames = 0;
	DWORD	flags = 0;

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
		_com_error	err(result);
		nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
		goto exit;
	}
	while (nativeDevice->capturingState.load(std::memory_order_acquire) == WASAPI_device::state::RUNNING) {
		nativeDevice.reset();
		nativeDevice = device.lock();
		if (!nativeDevice) { return; }
		result = nativeDevice->capture_client->GetNextPacketSize(&padding);
		if (FAILED(result)) {
			_com_error	err(result);
			nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
			goto exit;
		}
		if (padding != 0) {
			result = nativeDevice->capture_client->GetBuffer(&buffer,
				&padding,
				&flags,
				nullptr,
				nullptr);
			if (FAILED(result) || buffer == nullptr) {
				_com_error	err(result);
				nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
				goto exit;
			}
			nativeDevice->clientDevice.signals.captureStreamSig(reinterpret_cast<float*>(buffer), padding);
			result = nativeDevice->capture_client->ReleaseBuffer(padding);
			if (FAILED(result)) {
				_com_error	err(result);
				nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
				goto exit;
			}
		}
		else {
			std::this_thread::yield();
		}
	}
	result = nativeDevice->audio_client->Stop();
	if (FAILED(result)) {
		_com_error	err(result);
		nativeDevice->clientDevice.signals.deviceErrorSig("Audio Client: " + std::string(err.ErrorMessage()));
	}
exit:
	nativeDevice->capturingState.store(WASAPI_device::state::OFF, std::memory_order_release);
	nativeDevice.reset();
	if (task) { AvRevertMmThreadCharacteristics(task); }
}

netero::audio::RtCode   netero::audio::backend::deviceStartRecording(const netero::audio::device &device) {
	HRESULT	result;

	auto nativeDevice = pImpl->WASAPI_get_device(device);
	if (!nativeDevice) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (nativeDevice->capturingState.load(std::memory_order_acquire) == WASAPI_device::state::RUNNING) {
		return RtCode::ERR_ALREADY_RUNNING;
	}


	nativeDevice->capturingState.store(WASAPI_device::state::RUNNING, std::memory_order_release);
	nativeDevice->capturingThread = std::make_unique<std::thread>(capturingThreadHandle, nativeDevice);
    return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::deviceStopRecording(const netero::audio::device &device) {
	auto nativeDevice = pImpl->WASAPI_get_device(device);
	if (!nativeDevice) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (nativeDevice->capturingState.load(std::memory_order_acquire) != WASAPI_device::state::RUNNING) {
		if (nativeDevice->capturingThread) {
			nativeDevice->capturingThread->join();
			nativeDevice->capturingThread.reset();
		}
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	nativeDevice->capturingState.store(WASAPI_device::state::STOP, std::memory_order_release);
	std::chrono::time_point<std::chrono::system_clock> start, end = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() < 1000) {
		if (nativeDevice->capturingState.load(std::memory_order_acquire) == WASAPI_device::state::OFF) {
			break;
		}
		end = std::chrono::system_clock::now();
	}
	nativeDevice->capturingThread->join();
	nativeDevice->capturingThread.reset();
    return RtCode::OK;
}

