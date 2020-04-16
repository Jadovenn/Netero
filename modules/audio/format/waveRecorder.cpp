/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/audio/format/waveRecorder.hpp>

netero::audio::waveRecorder::waveRecorder(const std::string& name)
    : _fileName(name + ".wav")
{
    _waveFileHeader.header.FileSize = sizeof(WaveHeader) - 8;
    _waveFileHeader.format.ChannelNbr = _format.channels;
    _waveFileHeader.format.AudioFormat = WaveAudioFormat::IEEE_FLOAT;
    _waveFileHeader.format.Frequency = _format.samplingFrequency;
    _waveFileHeader.format.BytePerSec = _format.samplingFrequency * _format.bytesPerFrame;
    _waveFileHeader.format.BytePerBlock = _format.bytesPerFrame;
    _waveFileHeader.format.BitsPerSample = _format.bytesPerSample * 8;
    _buffer.reset(_waveFileHeader.format.BytePerSec);
    this->captureSlot.set<waveRecorder>(&waveRecorder::captureStream, this);
    this->deviceDisconnectedSlot.set(&waveRecorder::onDisconnected, this);
}

netero::audio::waveRecorder::operator bool() const {
    return _fileStream.is_open();
}

void    netero::audio::waveRecorder::onDisconnected(const std::string&) {
    pause();
}

void    netero::audio::waveRecorder::setFormat(const StreamFormat &format) {
    _format = format;
    _waveFileHeader.header.FileSize = sizeof(WaveHeader) - 8;
    _waveFileHeader.format.ChannelNbr = _format.channels;
    _waveFileHeader.format.AudioFormat = WaveAudioFormat::IEEE_FLOAT;
    _waveFileHeader.format.Frequency = _format.samplingFrequency;
    _waveFileHeader.format.BytePerSec = _format.samplingFrequency * _format.bytesPerFrame;
    _waveFileHeader.format.BytePerBlock = _format.bytesPerFrame;
    _waveFileHeader.format.BitsPerSample = _format.bytesPerSample * 8;
    _buffer.reset(_waveFileHeader.format.BytePerSec);
}

void    netero::audio::waveRecorder::captureStream(const float* buffer, const size_t frames) {
    size_t writtenBlocks = 0;
    const size_t blocks = frames * _format.channels;

    if (_state.load(std::memory_order_acquire) == state::PAUSED) { return; }
    if (_state.load(std::memory_order_acquire) == state::RECORDING) {
        while (writtenBlocks != blocks) {
            writtenBlocks = _buffer.write(buffer + writtenBlocks, blocks - writtenBlocks);
        }
    }
}

int    netero::audio::waveRecorder::captureAsyncHandler() {
    size_t  writtenSize = 0;
    const size_t  size = _buffer.getSize();
    const auto tmpBuffer = new (std::nothrow) float[size];
    if (!tmpBuffer) {
        throw std::bad_alloc();
    }

    while (_state.load(std::memory_order_acquire) != state::OFF) {
        const size_t availableBlocks = _buffer.getPadding();

        const size_t blocksRead = _buffer.read(tmpBuffer, availableBlocks);
        _fileStream.write(reinterpret_cast<char*>(tmpBuffer), blocksRead * sizeof(float));
        writtenSize += blocksRead;
    }
    while (_buffer.getPadding() != 0) {
        const size_t availableBlocks = _buffer.getPadding();

        const size_t blocksRead = _buffer.read(tmpBuffer, availableBlocks);
        _fileStream.write(reinterpret_cast<char*>(tmpBuffer), blocksRead * sizeof(float));
        writtenSize += blocksRead;
    }
    return writtenSize;
}

void    netero::audio::waveRecorder::record() {
	if (_state.load(std::memory_order_acquire) == state::PAUSED) {
        this->_state.store(state::RECORDING, std::memory_order_release);
        return;
	}
    if (_state.load(std::memory_order_acquire) == state::RECORDING) { return;}
    _fileStream.open(_fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    _fileStream.seekp(0);
    _fileStream.write(reinterpret_cast<char *>(&_waveFileHeader), sizeof(_waveFileHeader));
    _state.store(state::RECORDING, std::memory_order_release);
    _recordingResult = std::async(std::launch::async,
        &netero::audio::waveRecorder::captureAsyncHandler,
        this);
}

void netero::audio::waveRecorder::pause() {
    if (_state.load(std::memory_order_acquire) == state::RECORDING) {
        _state.store(state::PAUSED, std::memory_order_release);
    }
}

void    netero::audio::waveRecorder::stop() {
    if (_state.load(std::memory_order_acquire) == state::OFF) { return; }
    _state.store(state::OFF, std::memory_order_release);
    _recordingResult.wait();
    const int written_size = _recordingResult.get();
    _waveFileHeader.header.FileSize = written_size * sizeof(float) + sizeof(WaveHeader) - 8;
    _waveFileHeader.data.DataSize = written_size * sizeof(float);
    _fileStream.seekp(0);
    _fileStream.write(reinterpret_cast<char *>(&_waveFileHeader), sizeof(_waveFileHeader));
    _fileStream.flush();
    _fileStream.close();
}

