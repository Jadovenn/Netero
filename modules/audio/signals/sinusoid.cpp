/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/audio/signals.hpp>

netero::audio::signals::sinusoid::sinusoid(const float amplitude,
    const float frequency,
    const float phase)
    :   _amplitude(amplitude),
		_actualAmplitude(0),
        _frequency(frequency),
        _phase(phase),
        _pulsation(0),
        _delta(0)
{
    _pulsation = (2 * netero::numbers::pi * _frequency) / static_cast<float>(_format.samplingFrequency);
    this->renderSlot.set(&sinusoid::renderStream, this);
}

netero::audio::signals::sinusoid::~sinusoid() = default;

void    netero::audio::signals::sinusoid::setFormat(const netero::audio::StreamFormat &format) {
    if (_format.samplingFrequency != format.samplingFrequency) {
        _format = format;
        _pulsation = ((2 * netero::numbers::pi * _frequency) / static_cast<float>(_format.samplingFrequency));
    }
}

void  netero::audio::signals::sinusoid::renderStream(float *buffer, size_t frames) {
    for (auto idx = 0; static_cast<unsigned>(idx) <  frames * _format.channels; idx++) {
        const float value = _actualAmplitude * sin(_pulsation * (_delta / static_cast<float>(_format.channels)) + _phase);
        buffer[idx] = value;
        if (static_cast<unsigned>(_delta) > _format.samplingFrequency / 2 && value == 0) {
            _delta = 0;
        }
        _delta += 1;
    }
}

void    netero::audio::signals::sinusoid::play() {
    if (_actualAmplitude == 0.0F) {
        _actualAmplitude = _amplitude;
    }
}

void    netero::audio::signals::sinusoid::pause() {
    stop();
}

void    netero::audio::signals::sinusoid::stop() {
    _actualAmplitude = 0.0F;
}

