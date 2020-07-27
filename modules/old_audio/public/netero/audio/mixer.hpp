/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file mixer.hpp
 * @brief Contain audio mixer declaration
 */

#include <list>
#include <mutex>

#include <netero/audio/device.hpp>
#include <netero/audio/renderStream.hpp>

namespace netero::audio {

/**
     * @brief Mix different RenderStream into a single one.
     * Mixer is the back-bones of the audio management, it servers as node
     * where RenderStream can connect. The audio rendering process can be see as a general tree.
     */
class mixer: public RenderStream {
    public:
    mixer();
    mixer(const mixer&) = delete;
    mixer(mixer&&) noexcept = delete;
    mixer& operator=(const mixer&) = delete;
    mixer& operator=(mixer&&) = delete;
    virtual ~mixer();

    /**
     * @brief Render audio signal in the given buffer.
     * @param[out] buffer to put rendered data in.
     * @param[in] frames number inside the given buffer.
     */
    void renderStream(float* buffer, size_t frames) final;

    /**
     * @brief Broadcast play call to sub stream.
     */
    void play() final;

    /**
     * @brief Broadcast pause call to sub stream.
     */
    void pause() final;

    /**
     * @brief Broadcast stop call to sub stream.
     */
    void stop() final;

    /**
     * @brief Set the given format as reference for the signal processing.
     * @param[in] streamFormat to update and use as reference
     * The mixer will propagate this event to all registered sub stream.
     */
    void setFormat(const netero::audio::StreamFormat& streamFormat) final;

    /**
     * Connect a stream to this mixer. The stream will be
     * render immediately by the mixer.
     * @param[in] stream to register in.
     */
    void addRenderStream(RenderStream* stream);

    /**
     * Disconnect the given stream from the mixer. The stream
     * will not rendered by the mixer anymore.
     * @param[in] stream Stream to register in
     */
    void removeRenderStream(RenderStream* stream);

    protected:
    std::list<RenderStream*> _streams = {}; /**< Streams managed by the mixer. */

    private:
    /**
     * @brief Alloc an internal buffer for processing.
     * The internal buffer is used for the signal processing, it is allocated
     * while a new device is connected or while the stream format changed.
     */
    void alloc_internal_buffer() noexcept;

    /**
     * @brief Free the internal buffer.
     */
    void free_internal_buffer() noexcept;

    /**
     * @brief mix two signal into one.
     * @param[out] dest is a buffer contained data that will be mixed.
     * @param[in] source is a buffer that contain data to be mix in dest.
     * @param[in] sampleCount is the number of sample to read in each buffer. 
     */
    static void mix(float* __restrict dest, float* __restrict source, size_t sampleCount);

    netero::audio::device _device; /**< The connected device if any. */
    netero::audio::StreamFormat
               _streamFormat; /**< The reference stream format. May differ from the device one. */
    std::mutex _streamsGuard; /**< Mutex to avoid data race on the _streams vector. */
    size_t     _samplesCount = 0;       /**< Number of sample contained in the internal buffer. */
    float*     _sourceBuffer = nullptr; /**< The internal buffer. */
};
} // namespace netero::audio
