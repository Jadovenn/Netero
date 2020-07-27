/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <numeric>
#include <algorithm>
#include <netero/audio/mixer.hpp>
#include <netero/audio/backend.hpp>

netero::audio::mixer::mixer() {
    this->formatChangedSlot.set(&mixer::setFormat, this);
    this->renderSlot.set(&mixer::renderStream, this);
}

netero::audio::mixer::~mixer() {
    this->stop();
    this->free_internal_buffer();
}

void netero::audio::mixer::setFormat(const netero::audio::StreamFormat& streamFormat) {
    this->free_internal_buffer();
    this->_streamFormat = streamFormat;
    this->alloc_internal_buffer();
    for (auto *stream: this->_streams) {
        stream->setFormat(_device.format);
    }
}

void    netero::audio::mixer::alloc_internal_buffer() noexcept {
    _samplesCount = static_cast<size_t>(_streamFormat.framesCount * _streamFormat.channels);
    _sourceBuffer = new (std::nothrow) float[_samplesCount];
    if (!_sourceBuffer) {
        _samplesCount = 0;
        _sourceBuffer = nullptr;
        return;
    }
    std::memset(_sourceBuffer, 0, _samplesCount * _streamFormat.bytesPerSample);
}

void    netero::audio::mixer::free_internal_buffer() noexcept {
    if (_sourceBuffer) {
		delete[] _sourceBuffer;
		_sourceBuffer = nullptr;
    }
}

//void netero::audio::mixer::mix(float *__restrict dest, float *__restrict source, size_t sampleCount) {
//    for (int i = 0; i < sampleCount; i++) {
//        float avg = (dest[i] + source[i]) / 2;
//        float sign = avg < 0 ? -1 : 1;
//        dest[i] = sign * (1 - std::pow(1 - sign * avg, 2));
//    }
//}

void netero::audio::mixer::mix(float* __restrict dest, float* __restrict source, size_t sampleCount) {
    for (size_t idx = 0; idx < sampleCount; idx++) {
        dest[idx] = std::min<float>(1.0 , std::max<float>(-1.0, dest[idx] + source[idx]));
    }
}

void  netero::audio::mixer::renderStream(float* buffer, size_t frames) {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    if (!_streams.empty() && _sourceBuffer) {
        for (auto* stream : _streams) {
            std::memset(_sourceBuffer, 0, frames * _streamFormat.bytesPerFrame);
            stream->renderStream(_sourceBuffer, frames);
            mixer::mix(buffer, _sourceBuffer, frames * _streamFormat.channels);
        }
    }
}

void    netero::audio::mixer::play() {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    for (auto* stream : _streams) {
        stream->play();
    }
}

void    netero::audio::mixer::pause() {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    for (auto* stream : _streams) {
        stream->pause();
    }
}

void    netero::audio::mixer::stop() {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    for (auto* stream : _streams) {
        stream->stop();
    }
}

void    netero::audio::mixer::addRenderStream(RenderStream* stream) {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    stream->setFormat(_streamFormat);
    _streams.push_back(stream);
}

void    netero::audio::mixer::removeRenderStream(RenderStream* stream) {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    _streams.remove(stream);
}

