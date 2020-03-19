/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <chrono>

#include "WASAPI.hpp"

netero::audio::RtCode   netero::audio::engine::impl::start() {
	HRESULT result;
	if (!_cb) {
		return ERR_MISSING_CALLBACK;
	}
	// fill first buffer using callback
	BYTE* buffer = nullptr;
	result = _audio_rendering->GetBuffer(_frameCount, &buffer);
	if (FAILED(result)) {
		return ERR_NATIVE;
	}
	_cb(reinterpret_cast<float*>(buffer), _frameCount);
	result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
	if (FAILED(result)) {
		return ERR_NATIVE;
	}
	DWORD taskIndex = 0;
	_task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!_task) {
		return ERR_NATIVE;
	}
	result = _audio_client->Start();
	if (FAILED(result)) {
		return ERR_NATIVE;
	}
	return OK;
}

netero::audio::RtCode   netero::audio::engine::impl::stop() {
	HRESULT result;
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < _latency / 10000) {
		std::this_thread::yield();
	}

	result = _audio_client->Stop();
	if (FAILED(result)) {
		return ERR_NATIVE;
	}
	if (_task) {
		AvRevertMmThreadCharacteristics(_task);
		_task = nullptr;
	}
	return OK;
}

netero::audio::RtCode   netero::audio::engine::impl::poll() {
	HRESULT result;
	BYTE* buffer = nullptr;
	DWORD ret = WaitForSingleObject(_event, 2000);
	if (ret != WAIT_OBJECT_0) { // Timeout
		return stop();
	}
	result = _audio_rendering->GetBuffer(_frameCount, &buffer);
	if (FAILED(result)) {
		return ERR_NATIVE;
	}
	_cb(reinterpret_cast<float*>(buffer), _frameCount);
	result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
	if (FAILED(result)) {
		return ERR_NATIVE;
	}
	return OK;
}

