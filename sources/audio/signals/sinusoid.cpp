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
        _format{},
        _audio_device(netero::audio::device::GetAudioDevice())
{
    _format = _audio_device.getWaveFormat();
    _pulsation = ((2 * M_PI * _frequency) / _format.samplingFrequency);
}

netero::audio::signals::sinusoid::~sinusoid() {
}

void    netero::audio::signals::sinusoid::setFormat(netero::audio::WaveFormat &format) {
    if (_format.samplingFrequency != format.samplingFrequency) {
        _format = format;
        _pulsation = ((2 * M_PI * _frequency) / _format.samplingFrequency);
    }
}

void  netero::audio::signals::sinusoid::render(float *buffer, size_t size) {
    for (int idx = 0; idx < size * 2; idx++, _delta++) {
        float value = _amplitude * sin(_pulsation * (_delta / _format.channels) + _phase);
        buffer[idx] = value;
        if (_delta > 500 && value == 0) {
            _delta = 0;
        }
    }
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

