/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file buffer.hpp
 * @brief Thread safe circular buffer.
 */

#include <algorithm>
#include <cstring>
#include <exception>
#include <mutex>
#include <type_traits>

namespace netero {

	/**
	 * @brief Thread safe circular buffer.
	 * Buffer that support easy read and write in different threads.
	 * It is implemented as a circular buffer, a read or write call could fail or not
	 * write all block from the request. Be careful of returned values.
     * @warning The size of the buffer is counted in block not byte.
     *          For example an 32 bits integer will default construct a 4 blocks large buffer.
	 */
    template<class T,
        typename = std::enable_if<std::is_copy_assignable<T>::value>,
        typename = std::enable_if<std::is_default_constructible<T>::value>>
    class shared_buffer {
    public:
    	/**
    	 * The default constructor construct a buffer of size sizeof(T)
    	 * @param size is the number of block to allocate.
    	 * @warning May throw a bad_alloc exception!
    	 */
        explicit shared_buffer(const size_t size = sizeof(T))
            :   _size(size),
                _readOffset(-1),
                _writeOffset(0)
        {
            _buffer = new (std::nothrow) T[_size];
            if (!_buffer) {
                _buffer = nullptr;
                throw std::bad_alloc();
            }
        }

        shared_buffer(shared_buffer &copy) {
            std::scoped_lock<std::mutex> lock(copy._bufferMutex);

            this->_size = copy._size;
            _buffer = new (std::nothrow) T[_size];
            if (!_buffer) {
                _buffer = nullptr;
                throw std::bad_alloc();
            }
            std::memcpy(this->_buffer, copy._buffer, _size * sizeof(T));
            this->_writeOffset = copy._writeOffset;
            this->_readOffset = copy._readOffset;
        }

        shared_buffer(shared_buffer&& move) noexcept {
            std::scoped_lock<std::mutex> lock(move._bufferMutex);
            this->_buffer = move._buffer;
            this->_size = move._size;
            this->_readOffset = move._readOffset;
            this->_writeOffset = move._writeOffset;
            move._buffer = nullptr;
            move._size = 0;
            move._writeOffset = 0;
            move._readOffset = 0;
        }

    	/**
    	 * @brief Copy operator, is deleted, because it could not safely lock copy mutex.
    	 */
        shared_buffer& operator=(const shared_buffer& copy) = delete;

        /**
         * @brief Move on assign operator.
         * @warning The buffer of the container will be deleted and replaced.
         */
        shared_buffer& operator=(shared_buffer&& move) noexcept {
            std::scoped_lock<std::mutex>    lock(this->_bufferMutex);
            std::scoped_lock<std::mutex>    otherLock(move._bufferMutex);
            delete _buffer;
            this->_buffer = move._buffer;
            this->_size = move._size;
            this->_readOffset = move._readOffset;
            this->_writeOffset = move._writeOffset;
            move._buffer = nullptr;
            move._size = 0;
            move._writeOffset = 0;
            move._readOffset = 0;
            return *this;
        }

    	/**
    	 * Two container are equal if they point to the same buffer.
    	 */
        bool    operator==(const shared_buffer& other) const {
            return _buffer == other._buffer;
        }

    	/**
    	 * @brief Boolean test operator.
    	 * A container is considered invalid if its buffer is null or size equal to 0.
    	 */
        explicit operator bool() const {
            return !(_buffer == nullptr || _size == 0);
        }

        ~shared_buffer() {
            delete _buffer;
        }

    	/**
    	 * @brief Size getter.
    	 * Return the size of the internal buffer in block.
    	 * @attention To have the size in byte -> bloc * sizeof(T)
    	 */
        [[nodiscard]] size_t  getSize() const {
            return _size;
        }

    	/**
    	 * @brief Return the number of block valid for read operation.
    	 */
        [[nodiscard]] int getPadding() const {
            if (_size == 0) {
                return 0;
            }
        	if (_writeOffset == -1 && _readOffset == -1) {
                return _size;
        	}
            if (_writeOffset == -1 && _readOffset > 0) {
                return _size - _readOffset;
            }
            if (_readOffset == -1 && _writeOffset > -1) {
                return _writeOffset;
            }
            if (_readOffset < _writeOffset) {
                return _writeOffset - 1 - _readOffset;
            }
            return _size - 1 - _readOffset;
        }

    	/**
    	 * @brief Delete the internal buffer and assign a new size to it.
    	 * @warning The internal buffer will be deleted.
    	 * @warning May throw a bad_alloc exception!
    	 */
        void    reset(size_t block) {
            std::lock_guard<std::mutex> lock(this->_bufferMutex);
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

    	/**
    	 * @brief Clear the internal buffer. And reset any pending operation.
    	 * @warning Any data will be lost and the buffer is now ready for write operation.
    	 */
    	void    clear() {
            std::lock_guard<std::mutex> lock(this->_bufferMutex);
            std::memset(this->_buffer, 0, this->_size * sizeof(T));
            this->_readOffset = -1;
            this->_writeOffset = 0;
        }

    	/**
    	 * @brief Read some blocks from the internal buffer.
    	 * Perform a read operation. This will try to copy request size blocks from the internal buffer
    	 * to a provided buffer. The real copied size is returned and may be different from the request.
    	 */
        int     read(T* __restrict outBuffer, size_t blocks) {
            int readCount;

            std::scoped_lock	lock(_bufferMutex);
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
            else if (_readOffset < _writeOffset) { // read until write region
                const int availableBlocks = _writeOffset - 1 - _readOffset;
                readCount = std::min<int>(availableBlocks, blocks);
                std::memcpy(outBuffer, (_buffer + _readOffset + 1), readCount * sizeof(T));
                _readOffset += readCount;
            }
            else { // read to the end of the buffer
                const int availableBlocks = _size - 1 - _readOffset;
                readCount = std::min<int>(availableBlocks, blocks);
                std::memcpy(outBuffer, (_buffer + _readOffset + 1), readCount * sizeof(T));
                _readOffset += readCount;
                if (static_cast<size_t>(_readOffset) == _size - 1) {
                    _readOffset = -1;
                }
            }
            return readCount;
        }

        /**
         * @brief Write some blocks to the internal buffer.
         * Perform a write operation. This will try to write request size blocks to the internal buffer
         * from a provided buffer. The real written size is returned and may be different from the request.
         */
        int     write(const T* __restrict inBuffer, size_t blocks) {
            int writeCount;
            std::scoped_lock lock(_bufferMutex);
            if (!_buffer || _size == 0 || blocks == 0 || _readOffset == _writeOffset) {
                return 0;
            }
            if (_writeOffset < _readOffset) {
                const int availableBlocks = _readOffset - _writeOffset;
                writeCount = std::min<int>(availableBlocks, blocks);
                std::memcpy((_buffer + _writeOffset), inBuffer, writeCount * sizeof(T));
                _writeOffset += writeCount;
            }
            else {
                writeCount = std::min<int>(_size - _writeOffset, blocks);
                std::memcpy((_buffer + _writeOffset), inBuffer, writeCount * sizeof(T));
                _writeOffset += writeCount;
                if (static_cast<size_t>(_writeOffset) == _size) {
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
        std::mutex      _bufferMutex; /**< Mutex to protect concurrent access to the internal buffer. */
        size_t          _size; /**< Size in block of the internal buffer. */
        T*              _buffer = nullptr; /**< The internal buffer of type T*/
        int             _readOffset; /**< The read offset. */
        int             _writeOffset; /**< The the write offset. */
    };
}

