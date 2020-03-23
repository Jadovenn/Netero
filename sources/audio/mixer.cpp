/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <numeric>
#include <netero/audio/mixer.hpp>

netero::audio::mixer::mixer()
    :   _format{}
{}

void    netero::audio::mixer::setFormat(netero::audio::WaveFormat& format) {
    _format = format;
    _mixBuffer = new (std::nothrow) float[_format.samplePerSecond];
    _sourceBuffer = new (std::nothrow) float[_format.samplePerSecond];
    if (!_mixBuffer || !_sourceBuffer) {
        throw std::bad_alloc();
    }
}

void netero::audio::mixer::mix(float *__restrict dest, float *__restrict source, size_t min_size) {
    for (int i = 0; i < min_size; i++) {
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
            size_t min_size = size > _format.samplePerSecond ? _format.samplePerSecond : size;
            for (auto* stream : _streams) {
                std::memset(_sourceBuffer, 0, size * sizeof(float));
                stream->render(_sourceBuffer, size);
                mix(buffer, _sourceBuffer, min_size);
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
    stream->setFormat(_format);
    _pist.reserve(_streams.size());
}

void    netero::audio::mixer::disconnect(AudioStream* stream) {
    _streams.remove(stream);
    _pist.reserve(_streams.size());
}

