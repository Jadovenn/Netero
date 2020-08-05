/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/audio/waveFile.hpp>

namespace netero::audio {

WaveFile::WaveFile(): _isOpen(false), _mode(std::fstream::binary)
{
}

WaveFile::~WaveFile()
{
    if (this->_isOpen) {
        this->close();
    }
    else if (this->_file.is_open()) {
        this->_file.close();
    }
}

WaveFile::RtCode WaveFile::setFormat(const Format& other)
{
    if (this->_mode & std::fstream::in) {
        return RtCode::NOT_SUPPORTED;
    }
    this->_fileAudioFormat.samplingFrequency = other.samplingFrequency;
    this->_fileAudioFormat.channels = other.channels;
    this->_fileAudioFormat.bytesPerFrame = other.bytesPerFrame;
    this->_fileAudioFormat.bytesPerSample = other.bytesPerSample;
    return RtCode::SUCCESS;
}

bool WaveFile::is_open()
{
    return this->_isOpen && this->_file.is_open();
}

WaveFile::RtCode WaveFile::open(const std::string& name, std::ios_base::openmode mode)
{
    if (this->_isOpen) {
        return RtCode::ALREDY_OPEN;
    }
    if (mode & std::fstream::in && mode & std::fstream::out) {
        return RtCode::BI_DIRECTIONAL_DATAFLOW_NOT_SUPPORTED;
    }
    this->_mode = this->_mode | mode;
    this->_file.open(name, mode);
    if (!_file.is_open()) {
        return RtCode::SYSTEM_ERROR;
    }
    if (mode & std::fstream::in) {
        this->_file.read(reinterpret_cast<char*>(&this->_waveFileHeader), sizeof(WaveHeader));
        if (this->_waveFileHeader.header.FileTypeBlockID != RIFF_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (this->_waveFileHeader.header.FileTypeBlockID != WAVE_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (this->_waveFileHeader.format.FormatBlockID != WAVE_Format_Chunck_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (this->_waveFileHeader.data.DataBlockID != WAVE_Data_Chunk_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (this->_waveFileHeader.format.AudioFormat != WaveAudioFormat::IEEE_FLOAT) {
            return RtCode::ENCODE_FORMAT_NOT_SUPPORTED;
        }
        this->_fileAudioFormat.channels = this->_waveFileHeader.format.ChannelNbr;
        this->_fileAudioFormat.samplingFrequency = this->_waveFileHeader.format.Frequency;
        this->_fileAudioFormat.bytesPerFrame = this->_waveFileHeader.format.BytePerBlock;
        this->_fileAudioFormat.bytesPerSample = this->_waveFileHeader.format.BitsPerSample / 8;
    }
    if (mode & std::fstream::out) {
        this->_waveFileHeader.header.FileSize = sizeof(WaveHeader) - 8;
        this->_waveFileHeader.format.AudioFormat = WaveAudioFormat::IEEE_FLOAT;
        this->_waveFileHeader.format.ChannelNbr = this->_fileAudioFormat.channels;
        this->_waveFileHeader.format.Frequency = this->_fileAudioFormat.samplingFrequency;
        this->_waveFileHeader.format.BytePerSec =
            this->_fileAudioFormat.samplingFrequency * this->_fileAudioFormat.bytesPerFrame;
        this->_waveFileHeader.format.BytePerBlock = this->_fileAudioFormat.bytesPerFrame;
        this->_waveFileHeader.format.BitsPerSample = this->_fileAudioFormat.bytesPerSample * 8;
        this->_file.write(reinterpret_cast<char*>(&this->_waveFileHeader), sizeof(WaveHeader));
    }
    this->_isOpen = true;
    return RtCode::SUCCESS;
}

WaveFile::RtCode WaveFile::close()
{
    if (!this->_isOpen) {
        return RtCode::NOT_OPEN;
    }
    if (this->_mode & std::fstream::out) {
        this->_file.seekg(0, std::fstream::end);
        int length = this->_file.tellp();
        this->_file.seekg(0, std::fstream::beg);
        int dataLength = (length - sizeof(WaveHeader)) * sizeof(float);
        this->_waveFileHeader.header.FileSize = dataLength + sizeof(WaveHeader) - 8;
        this->_waveFileHeader.data.DataSize = dataLength;
        this->_file.write(reinterpret_cast<char*>(&this->_waveFileHeader), sizeof(WaveHeader));
        this->_file.flush();
    }
    this->_file.close();
    this->_isOpen = false;
    return RtCode::SUCCESS;
}

int WaveFile::read(float*, size_t)
{
    return 0;
}

int WaveFile::write(const float*, size_t)
{
    return 0;
}

} // namespace netero::audio
