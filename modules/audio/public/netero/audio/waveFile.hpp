/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file waveRecorder.hpp
 * @brief Wave recorder audio entity.
 */

#include <atomic>
#include <fstream>
#include <future>
#include <string>

#include <netero/audio/format.hpp>
#include <netero/audio/wave.hpp>

namespace netero::audio {

class WaveFile {
    public:
    enum class RtCode {
        SUCCESS = 0,
        ALREDY_OPEN = 1,
        NOT_OPEN = 2,
        FORMAT_ERROR = 3,
        SYSTEM_ERROR = 4,
        ENCODE_FORMAT_NOT_SUPPORTED = 5,
        BI_DIRECTIONAL_DATAFLOW_NOT_SUPPORTED = 6,
        NOT_SUPPORTED = 7
    };

    WaveFile();

    WaveFile(WaveFile&) = delete;
    WaveFile(WaveFile&&) = delete;
    WaveFile& operator=(const WaveFile&) = delete;
    WaveFile& operator=(WaveFile&&) = delete;
    virtual ~WaveFile();

    RtCode open(const std::string& name, std::ios_base::openmode mode);
    RtCode close();

    bool is_open();

    int read(float*, size_t);
    int write(const float*, size_t);

    [[nodiscard]] const Format& getFormat() const { return this->_fileAudioFormat; }
    RtCode                      setFormat(const Format&);

    private:
    bool                    _isOpen;
    std::ios_base::openmode _mode;

    WaveHeader _waveFileHeader;
    Format     _fileAudioFormat;

    std::string  _fileName;
    std::fstream _file;
};

} // namespace netero::audio