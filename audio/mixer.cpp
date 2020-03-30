/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <numeric>
#include <algorithm>
#include <netero/audio/mixer.hpp>
#include <netero/audio/backend.hpp>

netero::audio::mixer::mixer()
    :   _format{},
    	_samplesCount(0),
        _sourceBuffer(nullptr)
{
    _format = netero::audio::backend::GetInstance().getFormat();
    alloc_internal_buffer();
}

netero::audio::mixer::~mixer() {
    stop();
    free_internal_buffer();
}

void    netero::audio::mixer::setFormat(const netero::audio::WaveFormat& format) {
    if (_format.samplingFrequency != format.samplingFrequency) {
        _format = format;
        for (auto* stream : _streams) {
            stream->setFormat(_format);
        }
        free_internal_buffer();
        alloc_internal_buffer();
    }
}

void    netero::audio::mixer::alloc_internal_buffer() {
    _samplesCount = (size_t)_format.framesCount * (size_t)_format.channels;
    _sourceBuffer = new (std::nothrow) float[_samplesCount];
    if (!_samplesCount) {
        throw std::bad_alloc();
    }
    std::memset(_sourceBuffer, 0, _samplesCount * _format.bytesPerSample);
}

void    netero::audio::mixer::free_internal_buffer() {
    delete _sourceBuffer;
    _sourceBuffer = nullptr;
}

/**
void netero::audio::mixer::mix(float *__restrict dest, float *__restrict source, size_t sampleCount) {
    for (int i = 0; i < sampleCount; i++) {
        float avg = (dest[i] + source[i]) / 2;
        float signe = avg < 0 ? -1 : 1;
        dest[i] = signe * (1 - std::pow(1 - signe * avg, 2));
    }
}
*/

void netero::audio::mixer::mix(float* __restrict dest, float* __restrict source, size_t sampleCount) {
    for (size_t idx = 0; idx < sampleCount; idx++) {
        dest[idx] = std::min<float>(1.0 , std::max<float>(-1.0, dest[idx] + source[idx]));
    }
}

void  netero::audio::mixer::render(float* buffer, size_t frames) {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    if (!_streams.empty()) {
        for (auto* stream : _streams) {
            std::memset(_sourceBuffer, 0, frames * _format.bytesPerFrame);
            stream->render(_sourceBuffer, frames);
            mixer::mix(buffer, _sourceBuffer, frames * _format.channels);
        }
    }
}

void    netero::audio::mixer::play() {
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

void    netero::audio::mixer::connect(AudioOutStream* stream) {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    _streams.push_back(stream);
}

void    netero::audio::mixer::disconnect(AudioOutStream* stream) {
    const std::lock_guard<std::mutex>   lock(_streamsGuard);
    _streams.remove(stream);
}
