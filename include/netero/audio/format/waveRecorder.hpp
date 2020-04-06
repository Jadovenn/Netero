/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <queue>
#include <string>
#include <fstream>
#include <thread>
#include <future>
#include <atomic>

#include <netero/buffer.hpp>
#include <netero/observer/slots.hpp>
#include <netero/audio/audio.hpp>
#include <netero/audio/format/wave.hpp>
#include <netero/audio/captureStream.hpp>

namespace netero::audio {

    class waveRecorder : public CaptureStream {
        enum class state {
            OFF,
            RECORDING
        };
    public:
        waveRecorder(netero::audio::engine& engine,
            const netero::audio::device& device,
            const std::string& name);
        waveRecorder(netero::audio::engine& engine,
            const std::string& name);

        operator bool();
        ~waveRecorder();
        waveRecorder(waveRecorder&) = delete;
        waveRecorder(waveRecorder&&) = delete;
        waveRecorder& operator=(const waveRecorder&) = delete;
        waveRecorder& operator=(waveRecorder&&) = delete;


        void    onFormatChange(const StreamFormat&);

        virtual void captureStream(const float* buffer, const size_t frames) final;
        virtual void record() final;
        virtual void stop() final;

    private:
        netero::shared_buffer<float>   _buffer;
        std::atomic<state>  _state = state::OFF;
        std::future<int>    _recordingResult;
        StreamFormat        _format;
        WaveHeader          _waveFileHeader;
        std::string         _fileName;
        std::fstream        _fileStream;
        int                 captureAsyncHandler();
    };
}
