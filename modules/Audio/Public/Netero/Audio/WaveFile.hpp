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

#include <Netero/Audio/Format.hpp>
#include <Netero/Audio/Wave.hpp>

namespace Netero::Audio {

class WaveFile {
    public:
    enum class RtCode {
        SUCCESS = 0,
        ALREADY_OPEN = 1,
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

    RtCode Open(const std::string& name, std::ios_base::openmode mode);
    RtCode Close();

    bool IsOpen();

    RtCode Read(float*, size_t);
    RtCode Write(const float*, size_t);

    [[nodiscard]] const Format& GetFormat() const { return this->myFileAudioFormat; }
    RtCode                      SetFormat(const Format&);

    private:
    bool                    myIsOpen;
    std::ios_base::openmode myMode;

    WaveHeader myWaveFileHeader;
    Format     myFileAudioFormat;

    std::string  myFileName;
    std::fstream myFile;
};

} // namespace Netero::Audio