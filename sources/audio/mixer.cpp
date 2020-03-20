/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <numeric>
#include <netero/audio/mixer.hpp>

netero::audio::mixer::mixer()
    :   _format{}
{}

void    netero::audio::mixer::setFormat(netero::audio::WaveFormat& format) {
    _format = format;
    for (auto* stream : _streams) {
        stream->setFormat(_format);
    }
}

double  netero::audio::mixer::mix(int n, std::vector<double> &values) {
    double avg = std::accumulate(values.begin(), values.end(), (double)0.0) / n;
    double signe = avg < 0 ? -1 : 1;
    return signe * (1 - std::pow(1 - signe * avg, n));
}

double  netero::audio::mixer::render(int delta, int channel) {
    if (_streams.empty()) {
        return 0;
    }
    else if (_streams.size() == 1) {
        return _streams.front()->render(delta, channel);
    }
    else {
        _pist.clear();
        for (auto* stream : _streams) {
            _pist.push_back(stream->render(delta, channel));
        }
    }
    return mix(_pist.size(), _pist);
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

