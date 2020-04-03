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
        _delta(0),
        _samplesCount(0),
        _format{}
{
    _pulsation = ((2 * M_PI * _frequency) / _format.samplingFrequency);
}

netero::audio::signals::sinusoid::~sinusoid() {
}

void    netero::audio::signals::sinusoid::onFormatChange(const netero::audio::StreamFormat &format) {
    if (_format.samplingFrequency != format.samplingFrequency) {
        _format = format;
        _pulsation = ((2 * M_PI * _frequency) / _format.samplingFrequency);
    }
}

void  netero::audio::signals::sinusoid::renderStream(float *buffer, size_t frames) {
    for (int idx = 0; idx < frames * _format.channels; idx++, _delta++) {
        float value = _amplitude * sin(_pulsation * (_delta / _format.channels) + _phase);
        buffer[idx] = value;
        if (_delta > _format.samplingFrequency / 2 && value == 0) {
            _delta = 0;
        }
    }
}

void    netero::audio::signals::sinusoid::play() {
    if (_amplitude == 0.0F) {
        _amplitude = 0.01;
    }
}

void    netero::audio::signals::sinusoid::pause() {
    stop();
}

void    netero::audio::signals::sinusoid::stop() {
    _amplitude = 0.01;
}

