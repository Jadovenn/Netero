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
#include <netero/audio/engine.hpp>
#include <netero/audio/format/wave.hpp>

namespace netero::audio {

    class waveRecorder : public AudioInStream {
        enum class state {
            OFF,
            RECORDING
        };
    public:
        explicit waveRecorder(const std::string&);
        waveRecorder(waveRecorder&) = delete;
        waveRecorder& operator=(const waveRecorder&) = delete;
        ~waveRecorder();
        operator bool();

        void    connect(netero::audio::engine&);

        netero::slot<void(const WaveFormat&)>  onFormatChangeSlot;
        void    onFormatChange(const WaveFormat&);

        netero::slot<void(const float*, const size_t)>  captureStreamSlot;
        virtual void captureStream(const float* buffer, const size_t frames) final;
        virtual void record() final;
        virtual void stop() final;

    private:
        int                captureAsyncHandler();
        std::atomic<state>  _state = state::OFF;
        std::future<int>    _recordingResult;
        WaveFormat          _format;
        WaveHeader          _waveFileHeader;
        const std::string   _fileName;
        std::fstream        _fileStream;
        netero::shared_buffer<float>   _buffer;
    };
}
