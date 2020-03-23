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
        _samplesCount(0),
        _buffer(nullptr),
        _format{},
        _audio_device(netero::audio::device::GetAudioDevice())
{
    _format = _audio_device.getWaveFormat();
    alloc_internal_buffer();
}

netero::audio::signals::sinusoid::~sinusoid() {
    stop();
    free_internal_buffer();
}

void    netero::audio::signals::sinusoid::alloc_internal_buffer() {
    _samplesCount = ((size_t)_format.framesCount) * ((size_t)_format.channels);
    _buffer = new (std::nothrow) float[_samplesCount];
    if (!_buffer) {
        throw std::bad_alloc();
    }
}

void    netero::audio::signals::sinusoid::free_internal_buffer() {
    delete _buffer;
    _buffer = nullptr;
}

void    netero::audio::signals::sinusoid::generate_signal() {
    _pulsation = ((2 * M_PI * _frequency) / _format.samplingFrequency);
    for (int idx = 0; idx < _samplesCount; idx++) {
        _buffer[idx] = _amplitude * sin(_pulsation * (idx / _format.channels) + _phase);
    }
}

void    netero::audio::signals::sinusoid::setFormat(netero::audio::WaveFormat &format) {
    if (_format.samplingFrequency != format.samplingFrequency) {
        free_internal_buffer();
        _format = format;
        alloc_internal_buffer();
        generate_signal();
    }
}

void  netero::audio::signals::sinusoid::render(float *buffer, size_t size) {
    std::memcpy(buffer, _buffer, (size_t)_format.framesCount * (size_t)_format.bytesPerFrame);
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

