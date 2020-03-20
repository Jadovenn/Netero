/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <list>

#include <netero/audio/audio.hpp>

namespace netero::audio {

    class mixer: public AudioStream {
    public:
        mixer();
        virtual ~mixer() = default;

        void    setFormat(WaveFormat&) override;
        double  render(int delta, int channel) override;
        void    play() override;
        void    pause() override;
        void    stop() override;

        void    connect(AudioStream*);
        void    disconnect(AudioStream*);

    private:
        double  mix(int n, std::vector<double>& values);
    protected:
        netero::audio::WaveFormat   _format;
    private:
        std::list<AudioStream*>     _streams;
        std::vector<double>         _pist;
    };
}
