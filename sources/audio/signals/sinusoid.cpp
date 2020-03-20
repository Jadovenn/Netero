/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>

netero::audio::signals::sinusoid::sinusoid()
    :   _amplitude(0.1),
        _frequency(50),
        _phase(0),
        _samplingFrequency(0),
        _pan(0),
        _pulsation(0),
        _isRunning(false)
{}

void    netero::audio::signals::sinusoid::setFormat(netero::audio::WaveFormat &format) {
    _samplingFrequency = format.samplePerSecond;
    _pulsation = ((2 * M_PI * _frequency) / _samplingFrequency);
}

// pan not yet computed
double  netero::audio::signals::sinusoid::render(int delta, int channel) {
    static double last_result = 0;
    if (_isRunning.load(std::memory_order_acquire) == false) {
        if (last_result != 0) {
            last_result = last_result > 0 ? last_result - 0.1 : last_result + 0.1;
        }
    }
    else {
        last_result = _amplitude * sin(_pulsation * delta + _phase);
    }
    return last_result;
}

void    netero::audio::signals::sinusoid::play() {
    _isRunning.store(true, std::memory_order_release);
}

void    netero::audio::signals::sinusoid::pause() {
    stop();
}

void    netero::audio::signals::sinusoid::stop() {
    _isRunning.store(false, std::memory_order_release);
}

