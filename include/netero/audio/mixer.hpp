/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <list>
#include <mutex>

#include <netero/audio/engine.hpp>
#include <netero/audio/audio.hpp>
#include <netero/observer/slots.hpp>

/**
 * @file mixer.hpp
 * @brief Containe audio mixer declaration
 */

namespace netero::audio {

    /**
     * @class mixer
     * @brief Mix different AudioStream into a single one.
     * Mixer is the back-bones of the audio management, it servers as node
     * where AudioStream can connect. The audio rendering process can be see as a general tree.
     */
    class mixer: public AudioOutStream {
    public:
        mixer();
        explicit mixer(engine&);
        explicit mixer(AudioOutStream&);
        virtual ~mixer();

        netero::slot<void(const WaveFormat&)> onFormatChangeSlot;
        void    onFormatChange(const WaveFormat&) override;
        void    renderStream(float* buffer, size_t frames) override;
        void    play() override;
        void    pause() override;
        void    stop() override;

        /**
         * @methode connect
         * Connect a stream to this mixer. The stream will be
         * render immediately by the mixer.
         * @param[in] stream Stream to register in.
         */
        void    connect(AudioOutStream *stream);

        /**
         * @methode disconnect
         * Disconnect the given stream from the mixer. The stream
         * will not rendered by the mixer anymore.
         * @param[in] stream Stream to register in
         */
        void    disconnect(AudioOutStream *stream);

    protected:
        netero::audio::WaveFormat   _format;
        std::list<AudioOutStream*>  _streams;

    private:
        void    alloc_internal_buffer();
        void    free_internal_buffer();
        static void    mix(float *__restrict dest, float *__restrict source, size_t min_size);
        std::mutex  _streamsGuard;
        size_t      _samplesCount;
        float       *_sourceBuffer;
    };
}
