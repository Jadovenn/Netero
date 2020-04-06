/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <exception>
#include <memory>
#include <mutex>
#include <type_traits>

namespace netero {

    template<class T,
        typename = std::enable_if<std::is_copy_assignable<T>::value>,
        typename = std::enable_if<std::is_default_constructible<T>::value>>
    class shared_buffer {
    public:
        explicit shared_buffer(size_t size = sizeof(T))
            :   _size(size),
                _readOffset(-1),
                _writeOffset(0)
        {
            _buffer = new (std::nothrow) T[_size];
            if (!_buffer) {
                throw std::bad_alloc();
            }
        }

        shared_buffer(shared_buffer &copy) {
            *this = copy;
        }

        shared_buffer(shared_buffer&& move) {
            std::scoped_lock(move._bufferMutex);
            this->_buffer = move._buffer;
            this->_size = move._size;
            this->_readOffset = move._readOffset;
            this->_writeOffset = move._writeOffset;
            delete move._buffer;
            move._buffer = nullptr;
            move._size = 0;
            move._writeOffset = 0;
            move._readOffset = 0;
        }

        shared_buffer& operator=(shared_buffer& copy) {
            std::scoped_lock(copy._bufferMutex);
            this->_size = copy._size;
            _buffer = new (std::nothrow) T[_size];
            if (!_buffer) {
                throw std::bad_alloc();
            }
            std::memcpy(this->_buffer, copy._buffer, _size * sizeof(T));
            this->_writeOffset = copy._writeOffset;
            this->_readOffset = copy._readOffset;
            return *this;
        }

        shared_buffer& operator=(shared_buffer&& move) {
            std::scoped_lock(move._bufferMutex);
            delete _buffer;
            this->_buffer = move._buffer;
            this->_size = move._size;
            this->_readOffset = move._readOffset;
            this->_writeOffset = move._writeOffset;
            delete move._buffer;
            move._buffer = nullptr;
            move._size = 0;
            move._writeOffset = 0;
            move._readOffset = 0;
            return *this;
        }

        bool    operator==(const shared_buffer& other) const {
            return _buffer == other._buffer;
        }

        operator bool() const {
            return !(_buffer == nullptr || _size == 0);
        }

        ~shared_buffer() {
            delete _buffer;
        }

        size_t  getSize() {
            return _size;
        }

        int getPadding() {
            if (_size == 0) {
                return 0;
            }
            else if (_writeOffset == -1) {
                return _size - _readOffset;
            }
            else if (_readOffset == -1 && _writeOffset > -1) {
                return _writeOffset;
            }
            if (_readOffset < _writeOffset) {
                return _writeOffset - 1 - _readOffset;
            }
            return _size - 1 - _readOffset;
        }

        void    reset(size_t block) {
            delete _buffer;
            _size = block;
            _readOffset = -1;
            _writeOffset = 0;
            _buffer = new (std::nothrow) T[_size];
            if (!_buffer) {
                _buffer = nullptr;
                throw std::bad_alloc();
            }
        }

        int     read(T* __restrict outBuffer, size_t blocks) {
            int readCount = 0;

            std::scoped_lock(_bufferMutex);
            if (!_buffer || _size == 0 || blocks == 0 || _readOffset == _writeOffset - 1) {
                return 0;
            }
            if (_readOffset == -1) {
                if (_writeOffset == -1) {
                    readCount = std::min<int>(_size, blocks);
                    _writeOffset = 0;
                }
                else {
                    readCount = std::min<int>(_writeOffset, blocks);
                }
                std::memcpy(outBuffer, _buffer, readCount * sizeof(T));
                _readOffset = readCount - 1;
            }
            else if (_readOffset < _writeOffset) { // read untile write region
                int availableBlocks = _writeOffset - 1 - _readOffset;
                readCount = std::min<int>(availableBlocks, blocks);
                std::memcpy(outBuffer, (_buffer + _readOffset + 1), readCount * sizeof(T));
                _readOffset += readCount;
            }
            else { // read to the end of the buffer
                int availableBlocks = _size - 1 - _readOffset;
                readCount = std::min<int>(availableBlocks, blocks);
                std::memcpy(outBuffer, (_buffer + _readOffset + 1), readCount * sizeof(T));
                _readOffset += readCount;
                if (_readOffset == _size - 1) {
                    _readOffset = -1;
                }
            }
            return readCount;
        }

        int     write(const T* __restrict inBuffer, size_t blocks) {
            int writeCount = 0;
            std::scoped_lock(_bufferMutex);
            if (!_buffer || _size == 0 || blocks == 0 || _readOffset == _writeOffset) {
                return 0;
            }
            if (_writeOffset < _readOffset) {
                int availableBlocks = _readOffset - _writeOffset;
                writeCount = std::min<int>(availableBlocks, blocks);
                std::memcpy((_buffer + _writeOffset), inBuffer, writeCount * sizeof(T));
                _writeOffset += writeCount;
            }
            else {
                writeCount = std::min<int>(_size - _writeOffset, blocks);
                std::memcpy((_buffer + _writeOffset), inBuffer, writeCount * sizeof(T));
                _writeOffset += writeCount;
                if (_writeOffset == _size) {
                    if (_readOffset == -1) {
                        _writeOffset = -1;
                    }
                    else {
                        _writeOffset = 0;
                    }
                }
            }
            return writeCount;
        }

    private:
        std::mutex      _bufferMutex;
        size_t          _size;
        T*              _buffer = nullptr;
        int             _readOffset;
        int             _writeOffset;
    };
}

