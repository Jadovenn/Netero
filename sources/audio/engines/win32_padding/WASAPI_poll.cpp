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

	// first fill completly our buffer
	_cb(_buffer, _bufferFrameCount);
	_rendering_idx = 0;

	// now grab the audio buffer and load first segment
	BYTE* buffer = nullptr;
	result = _audio_rendering->GetBuffer(_frameCount, &buffer);
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	std::memcpy(buffer, _buffer, _WASAPIBufferSize);
	_flushing_idx = 0;
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
	while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < _latency / 10000) {
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
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	buffer_flush(buffer, frames);
	buffer_render(frames);
	result = _audio_rendering->ReleaseBuffer(frames, 0);
	if (FAILED(result)) {
		return RtCode::ERR_NATIVE;
	}
	return RtCode::OK;
}

void	netero::audio::engine::impl::buffer_render(unsigned frames) {
	if (_rendering_idx < _flushing_idx && _rendering_idx + frames >= _flushing_idx) {
		_cb(_buffer + _rendering_idx, _flushing_idx - _rendering_idx);
		_rendering_idx += _flushing_idx - _rendering_idx;
	}
	else if (_rendering_idx < _bufferFrameCount && _bufferFrameCount + frames >= _bufferFrameCount) {
		_cb(_buffer + _rendering_idx, _bufferFrameCount - _rendering_idx);
		_rendering_idx = 0;
	}
	else {
		_cb(_buffer + _rendering_idx, frames);
		_rendering_idx += frames;
	}
	if (_rendering_idx >= _bufferFrameCount) {
		_rendering_idx = 0;
	}
}

void	netero::audio::engine::impl::buffer_flush(BYTE* dest, unsigned frames) {
	unsigned bytes = 0;;

//	if (_flushing_idx + frames > _rendering_idx) {
//		bytes = _rendering_idx - _flushing_idx * sizeof(float);
//		std::memcpy(dest, _buffer + _flushing_idx, bytes);
//		_flushing_idx += frames;
//	}
	if (_flushing_idx + frames > _bufferFrameCount) {
		bytes = (_bufferFrameCount - _flushing_idx) * sizeof(float);
		std::memcpy(dest, _buffer + _flushing_idx, bytes);
		_flushing_idx += frames;
		//buffer_flush(dest, frames - bytes / sizeof(float));
	}
	else {
		bytes = frames * sizeof(float);
		std::memcpy(dest, _buffer + _flushing_idx, bytes);
		_flushing_idx += frames;
	}
	if (_flushing_idx >= _bufferFrameCount) {
		_flushing_idx = 0;
	}
}

