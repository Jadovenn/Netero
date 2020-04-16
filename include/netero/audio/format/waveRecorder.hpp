/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file waveRecorder.hpp
 * @brief Wave recorder audio entity.
 */

#include <string>
#include <fstream>
#include <future>
#include <atomic>

#include <netero/buffer.hpp>
#include <netero/audio/audio.hpp>
#include <netero/audio/format/wave.hpp>
#include <netero/audio/captureStream.hpp>

namespace netero::audio {

	/**
	 * @brief Record audio input to a wave file.
     * The entity might be correctly connected to a device by
     * the audio engine. If you are using this entity without an audio engine,
     * make sure that you have set the format and connect the capture slot to an active
     * device before calling record().
     * @see netero::AudioStream
     * @see netero::CaptureStream
     * The wave record launch a thread to write on the fs.
	 */
    class waveRecorder : public CaptureStream {
        /**
         * State of the associated thread.
         */
        enum class state {
            OFF, /**< Not running. */
            RECORDING, /**< Actively writing on the filesystem. */
        	PAUSED, /**< The thread is running but not actively writing on the filesystem. */
        };
    public:
    	/**
    	 * @brief Constructor.
    	 * @param name of the file.
    	 */
        waveRecorder(const std::string&);
        waveRecorder(waveRecorder&) = delete;
        waveRecorder(waveRecorder&&) = delete;
        waveRecorder& operator=(const waveRecorder&) = delete;
        waveRecorder& operator=(waveRecorder&&) = delete;
        virtual ~waveRecorder() = default;

    	/**
    	 * @brief Test is the wave file is open.
    	 * @return True if it is open, false otherwise.
    	 */
        operator bool() const;


    	/**
    	 * @brief Set the audio signal format of the stream to capture.
    	 * This might be used by the audio engine. If you are using this
    	 * entity without an audio engine, make sure to set the format before start recording.
    	 */
        void    setFormat(const StreamFormat&) final;

    	/**
    	 * @brief Callback called if the active device is disconnected.
    	 * This while call pause().
    	 */
        void    onDisconnected(const std::string &);

    	/***
    	 * @brief Audio callback, that capture the audio signal.
    	 */
        void captureStream(const float* buffer, const size_t frames) final;

    	/**
    	 * @brief Open the file and start the capture.
    	 */
        void record() final;

    	/**
    	 * @brief Pause the capture, save the writing offset.
    	 */
        void pause() final;

    	/**
    	 * @brief Stop the capture, edit the wave header and close the file.
    	 */
        void stop() final;

    private:
        netero::shared_buffer<float>   _buffer; /**< Shared circular buffer. This is use to exchange data between fs and capture thread. */
        std::atomic<state>  _state = state::OFF; /**< State of the fs thread. */
        std::future<int>    _recordingResult; /**< Future of the fs async task. */
        StreamFormat        _format; /**< The stream format used by the device. */
		WaveHeader          _waveFileHeader; /**< The wave's file header. */
        std::string         _fileName; /**< The file's name. */
        std::fstream        _fileStream; /**< The file stream. */

    	/**
    	 * Handler for the fs async task.
    	 */
        int                 captureAsyncHandler();
    };
}

