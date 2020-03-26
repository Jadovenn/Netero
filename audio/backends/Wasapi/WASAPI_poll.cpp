/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <chrono>

#include "WASAPI.hpp"

netero::audio::RtCode   netero::audio::engine::impl::start() {
	HRESULT result;
	if (!_cb) {
		return RtCode::ERR_MISSING_CALLBACK;
	}

	// now grab the audio buffer and load first segment
	BYTE* buffer = nullptr;
	result = _audio_rendering->GetBuffer(_frameCount, &buffer);
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	_cb(reinterpret_cast<float*>(buffer), _frameCount);
	result = _audio_rendering->ReleaseBuffer(_frameCount, 0);
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	DWORD taskIndex = 0;
	_task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	if (!_task) {
		return RtCode::ERR_NATIVE;
	}
	result = _audio_client->Start();
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	return RtCode::OK;
}

netero::audio::RtCode   netero::audio::engine::impl::stop() {
	HRESULT result;
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() < _latency) {
		std::this_thread::yield();
	}

	result = _audio_client->Stop();
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	if (_task) {
		AvRevertMmThreadCharacteristics(_task);
		_task = nullptr;
	}
	return RtCode::OK;
}

netero::audio::RtCode   netero::audio::engine::impl::poll() {
	HRESULT result;
	BYTE* buffer = nullptr;
	unsigned padding = 0;
	unsigned frames = 0;

	result = _audio_client->GetCurrentPadding(&padding);
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	frames = _frameCount - padding;
	result = _audio_rendering->GetBuffer(frames, &buffer);
	if (FAILED(result) || buffer == nullptr) {
		return RtCode::ERR_NATIVE;
	}
	_cb(reinterpret_cast<float*>(buffer), frames);
	result = _audio_rendering->ReleaseBuffer(frames, 0);
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	return RtCode::OK;
}

