/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>

netero::audio::signals::sinusoid::sinusoid(double amplitude, double frequency, double phase)
    :   _amplitude(amplitude),
        _frequency(frequency),
        _phase(phase),
        _samplingFrequency(0),
        _pan(0),
        _pulsation(0),
        _buffer(nullptr),
        _format{},
        _audio_device(netero::audio::device::GetAudioDevice())
{}

netero::audio::signals::sinusoid::~sinusoid() {
    stop();
}

void    netero::audio::signals::sinusoid::setFormat(netero::audio::WaveFormat &format) {
    if (_format.samplePerSecond == format.samplePerSecond) {
        return;
    }
    _format = format;
    _samplingFrequency = format.samplePerSecond;
    _pulsation = ((2 * M_PI * _frequency) / _samplingFrequency);
    _buffer = new (std::nothrow) float[_format.framesCount];
    if (!_buffer) {
        throw std::bad_alloc();
    }
    for (int i = 0; i < _format.framesCount; i++) {
        _buffer[i] = _amplitude * sin(_pulsation * i + _phase);
    }
}

// pan not yet computed
void  netero::audio::signals::sinusoid::render(float *buffer, size_t size) {
    for (int i = 0, j =0; i < size / 2; i++, j += 2) {
        float current = _amplitude * sin(_pulsation * i + _phase);
        buffer[j] = _amplitude * sin(_pulsation * i + _phase);
        buffer[j+1] = _amplitude * sin(_pulsation * i + _phase);
    }
    return;
    /**
    if (!_buffer || _samplingFrequency == 0) {
        return;
    }
    if (_samplingFrequency >= size) {
        std::memcpy(buffer, _buffer, size * sizeof(float));
    }
    else {
        for (int i = 0; i < size; i += _samplingFrequency) {
            std::memcpy(buffer, _buffer, (int)_samplingFrequency * sizeof(float));
            if (i + _samplingFrequency >= size) {
                size_t last_bytes = (size - i) * sizeof(float);
                std::memcpy(buffer, _buffer, last_bytes);
            }
        }
    }
    */
}

void    netero::audio::signals::sinusoid::play() {
    _audio_device.connect(this);
}

void    netero::audio::signals::sinusoid::pause() {
    stop();
}

void    netero::audio::signals::sinusoid::stop() {
    _audio_device.disconnect(this);
}

