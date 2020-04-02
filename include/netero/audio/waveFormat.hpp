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
#include <netero/circular_buffer.hpp>
#include <netero/observer/slots.hpp>
#include <netero/audio/audio.hpp>
#include <netero/audio/engine.hpp>

namespace netero::audio {

#pragma pack(push, 1)

    struct WaveHeaderBlock {
        const uint32_t  FileTypeBlockID = 0x46464952;
        uint32_t        FileSize = 0;
        const uint32_t  FileFormatID = 0x45564157;
    };

    struct WaveFormatBlock {
        const uint32_t  FormatBlockID = 0x20746D66;
        const uint32_t  BlockSize = 0x10;
        uint16_t        AudioFormat = 1;
        uint16_t        ChannelNbr = 0;
        uint32_t        Frequence = 0;
        uint32_t        BytePerSec = 0;
        uint16_t        BytePerBlock = 0;
        uint16_t        BitsPerSample = 0;
    };

    struct WaveDataBlock {
        const uint32_t  DataBlockID = 0x61746164;
        uint32_t        DataSize = 0;
    };

    struct WaveHeader {
        WaveHeaderBlock header;
        WaveFormatBlock format;
        WaveDataBlock   data;
    };

#pragma pack(pop)

    class waveRecorder: public AudioInStream {
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
      
        void    connect(netero::audio::engine &);

        netero::slot<void(const WaveFormat &)>  onFormatChangeSlot;
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
        netero::shared_circular_buffer<float>   _buffer;
    };
}

