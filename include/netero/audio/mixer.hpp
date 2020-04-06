/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

/**
 * @file mixer.hpp
 * @brief Containe audio mixer declaration
 */

#include <list>
#include <mutex>

#include <netero/audio/renderStream.hpp>
#include <netero/observer/slots.hpp>

namespace netero::audio {

    /**
     * @class mixer
     * @brief Mix different RenderStream into a single one.
     * Mixer is the back-bones of the audio management, it servers as node
     * where RenderStream can connect. The audio rendering process can be see as a general tree.
     */
    class mixer: public RenderStream {
    public:
        mixer(netero::audio::engine&);
        mixer(netero::audio::engine&, const netero::audio::device& device);
        virtual ~mixer();

        void    onFormatChange(const StreamFormat&) override;
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
        void    add(RenderStream *stream);

        /**
         * @methode disconnect
         * Disconnect the given stream from the mixer. The stream
         * will not rendered by the mixer anymore.
         * @param[in] stream Stream to register in
         */
        void    remove(RenderStream *stream);

    protected:
        std::list<RenderStream*>        _streams;

    private:
        void    alloc_internal_buffer();
        void    free_internal_buffer();
        static void    mix(float *__restrict dest, float *__restrict source, size_t min_size);
        std::mutex  _streamsGuard;
        size_t      _samplesCount;
        float       *_sourceBuffer;
    };
}
