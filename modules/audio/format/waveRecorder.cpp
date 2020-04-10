/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <netero/audio/format/waveRecorder.hpp>

netero::audio::waveRecorder::waveRecorder(netero::audio::engine& engine,
    const netero::audio::device& device,
    const std::string& name)
    :   CaptureStream(engine, device),
        _fileName(name + ".wav")
{
    onFormatChangeSlot.set(&netero::audio::waveRecorder::onFormatChange, this);
    onDisconnectedSlot.set(&netero::audio::waveRecorder::onDisconnected, this);
    captureSlot.set(&netero::audio::waveRecorder::captureStream, this);
    _fileStream.open(_fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    _waveFileHeader.header.FileSize = sizeof(WaveHeader) - 8;
    _waveFileHeader.format.ChannelNbr = _device.format.channels;
    _waveFileHeader.format.AudioFormat = WaveAudioFormat::IEEE_FLOAT;
    _waveFileHeader.format.Frequence = _device.format.samplingFrequency;
    _waveFileHeader.format.BytePerSec = _device.format.samplingFrequency * _device.format.bytesPerFrame;
    _waveFileHeader.format.BytePerBlock = _device.format.bytesPerFrame;
    _waveFileHeader.format.BitsPerSample = _device.format.bytesPerSample * 8;
    _buffer.reset(_waveFileHeader.format.BytePerSec);
}

netero::audio::waveRecorder::waveRecorder(netero::audio::engine& engine,
    const std::string& name)
    :   CaptureStream(engine),
        _fileName(name + ".wav")
{
    onFormatChangeSlot.set(&netero::audio::waveRecorder::onFormatChange, this);
    captureSlot.set(&netero::audio::waveRecorder::captureStream, this);
    _fileStream.open(_fileName, std::ios::out | std::ios::trunc | std::ios::binary);

}

netero::audio::waveRecorder::~waveRecorder() {
    _fileStream.flush();
    _fileStream.close();
}

netero::audio::waveRecorder::operator bool() {
    return _fileStream.is_open();
}

void    netero::audio::waveRecorder::onDisconnected(const std::string& reason) {
    std::cout << "Audio device disconected: " << reason << std::endl;
    stop();
}

void    netero::audio::waveRecorder::onFormatChange(const StreamFormat &format) {
    _device.format = format;
    _waveFileHeader.header.FileSize = sizeof(WaveHeader) - 8;
    _waveFileHeader.format.ChannelNbr = _device.format.channels;
    _waveFileHeader.format.AudioFormat = WaveAudioFormat::IEEE_FLOAT;
    _waveFileHeader.format.Frequence = _device.format.samplingFrequency;
    _waveFileHeader.format.BytePerSec = _device.format.samplingFrequency * _device.format.bytesPerFrame;
    _waveFileHeader.format.BytePerBlock = _device.format.bytesPerFrame;
    _waveFileHeader.format.BitsPerSample = _device.format.bytesPerSample * 8;
    _buffer.reset(_waveFileHeader.format.BytePerSec);
}

void    netero::audio::waveRecorder::captureStream(const float* buffer, const size_t frames) {
    size_t writtenBlocks = 0;
    const size_t blocks = frames * _device.format.channels;

    if (_state.load(std::memory_order_acquire) == state::RECORDING) {
        while (writtenBlocks != blocks) {
            writtenBlocks = _buffer.write(buffer + writtenBlocks, blocks - writtenBlocks);
        }
    }
}

int    netero::audio::waveRecorder::captureAsyncHandler() {
    size_t  written_size = 0;
    size_t  size = _buffer.getSize();
    auto tmpBuffer = new (std::nothrow) float[size];
    if (!tmpBuffer) {
        throw std::bad_alloc();
    }

    while (_state.load(std::memory_order_acquire) == state::RECORDING) {
        size_t availableBlocks = _buffer.getPadding();
        size_t blocksRead = 0;

        blocksRead = _buffer.read(tmpBuffer, availableBlocks);
        _fileStream.write(reinterpret_cast<char*>(tmpBuffer), blocksRead * sizeof(float));
        written_size += blocksRead;
    }
    while (_buffer.getPadding() != 0) {
        size_t availableBlocks = _buffer.getPadding();
        size_t blocksRead = 0;

        blocksRead = _buffer.read(tmpBuffer, availableBlocks);
        _fileStream.write(reinterpret_cast<char*>(tmpBuffer), blocksRead * sizeof(float));
        written_size += blocksRead;
    }
    return written_size;
}

void    netero::audio::waveRecorder::record() {
    if (_state.load(std::memory_order_acquire) != state::OFF) {
        return;
    }
    _fileStream.seekp(0);
    _fileStream.write(reinterpret_cast<char *>(&_waveFileHeader), sizeof(_waveFileHeader));
    _state.store(state::RECORDING, std::memory_order_release);
    _recordingResult = std::async(std::launch::async,
        &netero::audio::waveRecorder::captureAsyncHandler,
        this);
}

void    netero::audio::waveRecorder::stop() {
    if (_state.load(std::memory_order_acquire) != state::RECORDING) {
        return;
    }
    _state.store(state::OFF, std::memory_order_release);
    _recordingResult.wait();
    int written_size = _recordingResult.get();
    _waveFileHeader.header.FileSize = written_size * sizeof(float) + sizeof(WaveHeader) - 8;
    _waveFileHeader.data.DataSize = written_size * sizeof(float);
    _fileStream.seekp(0);
    _fileStream.write(reinterpret_cast<char *>(&_waveFileHeader), sizeof(_waveFileHeader));
}

