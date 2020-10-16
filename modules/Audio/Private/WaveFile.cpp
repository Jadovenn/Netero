/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Audio/WaveFile.hpp>

namespace Netero::Audio {

WaveFile::WaveFile(): myIsOpen(false), myMode(std::fstream::binary)
{
}

WaveFile::~WaveFile()
{
    if (myIsOpen) {
        Close();
    }
    else if (myFile.is_open()) {
        myFile.close();
    }
}

WaveFile::RtCode WaveFile::SetFormat(const Format& aFormat)
{
    if (myMode & std::fstream::in) {
        return RtCode::NOT_SUPPORTED;
    }
    myFileAudioFormat.mySamplingFrequency = aFormat.mySamplingFrequency;
    myFileAudioFormat.myChannels = aFormat.myChannels;
    myFileAudioFormat.myBytesPerFrame = aFormat.myBytesPerFrame;
    myFileAudioFormat.myBytesPerSample = aFormat.myBytesPerSample;
    return RtCode::SUCCESS;
}

bool WaveFile::IsOpen()
{
    return myIsOpen && myFile.is_open();
}

WaveFile::RtCode WaveFile::Open(const std::string& aName, std::ios_base::openmode aMode)
{
    if (myIsOpen) {
        return RtCode::ALREADY_OPEN;
    }
    if (aMode & std::fstream::in && aMode & std::fstream::out) {
        return RtCode::BI_DIRECTIONAL_DATAFLOW_NOT_SUPPORTED;
    }
    myMode = myMode | aMode;
    myFile.open(aName, aMode);
    if (!myFile.is_open()) {
        return RtCode::SYSTEM_ERROR;
    }
    if (myMode & std::fstream::in) {
        myFile.read(reinterpret_cast<char*>(&myWaveFileHeader), sizeof(WaveHeader));
        if (myWaveFileHeader.header.FileTypeBlockID != RIFF_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (myWaveFileHeader.header.FileTypeBlockID != WAVE_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (myWaveFileHeader.format.FormatBlockID != WAVE_Format_Chunck_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (myWaveFileHeader.data.DataBlockID != WAVE_Data_Chunk_Magic) {
            return RtCode::FORMAT_ERROR;
        }
        if (myWaveFileHeader.format.AudioFormat != WaveAudioFormat::IEEE_FLOAT) {
            return RtCode::ENCODE_FORMAT_NOT_SUPPORTED;
        }

        myFileAudioFormat.myChannels = myWaveFileHeader.format.ChannelNbr;
        myFileAudioFormat.mySamplingFrequency = myWaveFileHeader.format.Frequency;
        myFileAudioFormat.myBytesPerFrame = myWaveFileHeader.format.BytePerBlock;
        myFileAudioFormat.myBytesPerSample = myWaveFileHeader.format.BitsPerSample / 8;
    }
    if (aMode & std::fstream::out) {
        myWaveFileHeader.header.FileSize = sizeof(WaveHeader) - 8;
        myWaveFileHeader.format.AudioFormat = WaveAudioFormat::IEEE_FLOAT;
        myWaveFileHeader.format.ChannelNbr = myFileAudioFormat.myChannels;
        myWaveFileHeader.format.Frequency = myFileAudioFormat.mySamplingFrequency;
        myWaveFileHeader.format.BytePerSec =
            myFileAudioFormat.mySamplingFrequency * myFileAudioFormat.myBytesPerFrame;
        myWaveFileHeader.format.BytePerBlock = myFileAudioFormat.myBytesPerFrame;
        myWaveFileHeader.format.BitsPerSample = myFileAudioFormat.myBytesPerSample * 8;
        myFile.write(reinterpret_cast<char*>(&myWaveFileHeader), sizeof(WaveHeader));
    }
    myIsOpen = true;
    return RtCode::SUCCESS;
}

WaveFile::RtCode WaveFile::Close()
{
    if (!myIsOpen) {
        return RtCode::NOT_OPEN;
    }
    if (myMode & std::fstream::out) {
        myFile.seekg(0, std::fstream::end);
        int length = myFile.tellp();
        myFile.seekg(0, std::fstream::beg);
        int dataLength = (length - sizeof(WaveHeader));
        myWaveFileHeader.header.FileSize = dataLength + sizeof(WaveHeader) - 8;
        myWaveFileHeader.data.DataSize = dataLength;
        myFile.write(reinterpret_cast<char*>(&myWaveFileHeader), sizeof(WaveHeader));
        myFile.flush();
    }
    myFile.close();
    myIsOpen = false;
    return RtCode::SUCCESS;
}

WaveFile::RtCode WaveFile::Read(float* outBuffer, size_t size)
{
    if (!(myMode & std::fstream::in)) {
        return RtCode::NOT_SUPPORTED;
    }
    myFile.read(reinterpret_cast<char*>(outBuffer), size * sizeof(float));
    return RtCode::SUCCESS;
}

WaveFile::RtCode WaveFile::Write(const float* inBuffer, size_t size)
{
    if (!(myMode & std::fstream::out)) {
        return RtCode::NOT_SUPPORTED;
    }
    myFile.write(reinterpret_cast<const char*>(inBuffer), size * sizeof(float));
    return RtCode::SUCCESS;
}

} // namespace Netero::Audio
