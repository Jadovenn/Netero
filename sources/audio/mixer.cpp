/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <numeric>
#include <netero/audio/mixer.hpp>
#include <netero/audio/device.hpp>

netero::audio::mixer::mixer()
    :   _samplesCount(0),
        _sourceBuffer(nullptr),
        _audio_device(netero::audio::device::GetAudioDevice())
{
    _format = _audio_device.getWaveFormat();
    alloc_internal_buffer();
}

netero::audio::mixer::~mixer() {
    stop();
    free_internal_buffer();
}

// this might be a very heavy call, sub stream may reallocate too
void    netero::audio::mixer::setFormat(netero::audio::WaveFormat& format) {
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

void netero::audio::mixer::mix(float *__restrict dest, float *__restrict source, size_t sampleCount) {
    for (int i = 0; i < sampleCount; i++) {
        float avg = (dest[i] + source[i]) / 2;
        float signe = avg < 0 ? -1 : 1;
        dest[i] = signe * (1 - std::pow(1 - signe * avg, 2));
    }
}

void  netero::audio::mixer::render(float* buffer, size_t size) {
    if (!_streams.empty()) {
        if (_streams.size() == 1) {
            _streams.front()->render(buffer, size);
        }
        else {
            for (auto* stream : _streams) {
                std::memset(_sourceBuffer, 0, _samplesCount * _format.bytesPerSample);
                stream->render(_sourceBuffer, size);
                mix(buffer, _sourceBuffer, _samplesCount);
            }
        }
    }
}

void    netero::audio::mixer::play() {
    for (auto* stream : _streams) {
        stream->play();
    }
}

void    netero::audio::mixer::pause() {
    for (auto* stream : _streams) {
        stream->pause();
    }
}

void    netero::audio::mixer::stop() {
    for (auto* stream : _streams) {
        stream->stop();
    }
}

void    netero::audio::mixer::connect(AudioStream* stream) {
    _streams.push_back(stream);
}

void    netero::audio::mixer::disconnect(AudioStream* stream) {
    _streams.remove(stream);
}

