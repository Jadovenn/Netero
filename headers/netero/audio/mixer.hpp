/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <list>
#include <mutex>

#include <netero/audio/audio.hpp>

namespace netero::audio {

    class mixer: public AudioStream {
    public:
        mixer();
        virtual ~mixer();

        void    setFormat(WaveFormat&) override;
        void    render(float* buffer, size_t size) override;
        void    play() override;
        void    pause() override;
        void    stop() override;

        void    connect(AudioStream*);
        void    disconnect(AudioStream*);

    private:
        std::mutex  _streamsGuard;
        void    alloc_internal_buffer();
        void    free_internal_buffer();
        void    mix(float *__restrict dest, float *__restrict source, size_t min_size);
    protected:
        netero::audio::WaveFormat   _format;
        std::list<AudioStream*>     _streams;
    private:
        size_t                      _samplesCount;
        float                       *_sourceBuffer;
    };
}
