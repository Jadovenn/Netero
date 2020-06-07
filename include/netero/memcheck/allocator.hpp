/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#pragma once

#include <limits>
#include <iostream>
#include <new>

namespace netero::memck {
    
    struct MemoryPool {
        void*       owner;
        std::size_t bytes;
    };

    class DefaultAllocatorReporter {
    public:
        static void reportAllocation(void*, MemoryPool*, void*, std::size_t) {}
        static void reportDeallocation(void*, MemoryPool*, void*, std::size_t) {}
        static void reportLeaks(void*, MemoryPool*) {}
        static void reportZeroSizeUndefinedBehavior(void*) {}
        static void reportNoByteInPool(void*) {}
    };

    template<typename T, class Reporter = DefaultAllocatorReporter>
    class Allocator {
    public:
        typedef T               value_type;
        typedef std::size_t     size_type;
        typedef std::ptrdiff_t  difference_type;
        typedef std::true_type  propagate_on_container_mode_assignment;
        typedef std::false_type is_always_equal;

        MemoryPool* pool;

        Allocator() {
            pool = static_cast<MemoryPool*>(std::malloc(sizeof(MemoryPool)));
            pool->bytes = 0;
            pool->owner = this;
        }

        template<typename U>
        Allocator(const Allocator<U, Reporter>& other) {
            this->pool = other.pool;
        }

        virtual ~Allocator() noexcept {
            if (this == pool->owner) {
                if (pool->bytes != 0) {
                    Reporter::reportLeaks(this, this->pool);
                }
                try {
                    std::free(pool);
                }
                catch (...) {
                    std::cerr << "Free failed there is nothing you can do." << std::endl;
                }
            }
        }

        [[nodiscard]] constexpr T* allocate(std::size_t size) {
            if (size == 0) {
                Reporter::reportZeroSizeUndefinedBehavior(this);
                return nullptr;
            }
            if (std::numeric_limits<std::size_t>::max() / sizeof(T) < size) {
                throw std::bad_array_new_length();
            }
            T* data = static_cast<T*>(std::malloc(sizeof(T) * size));
            if (!data) {
                throw std::bad_alloc();
            }
            Reporter::reportAllocation(this, this->pool, static_cast<void*>(data), size);
            pool->bytes += size * sizeof(T);
            return data;
        }

        constexpr void deallocate(T* data, std::size_t size) {
            if (!data) {
                return; // trying to delete a nullptr is a valid behavior in c++
            }
            if (size == 0) {
            Reporter::reportZeroSizeUndefinedBehavior(this);
            }
            if (pool->bytes == 0) {
                Reporter::reportNoByteInPool(this);
            }
            Reporter::reportDeallocation(this, this->pool, static_cast<void*>(data), size);
            std::free(data);
            this->pool->bytes -= sizeof(T) * size;
        }

        template<typename U>
        struct rebind {
            typedef Allocator<U, Reporter>  other;
        };

    };

    template< class T1, class T2 >
    bool operator!=(const Allocator<T1>& lhs, const Allocator<T2>& rhs) noexcept {
        return &lhs != &rhs;
    }

    template< class T1, class T2 >
    bool operator==(const Allocator<T1>& lhs, const Allocator<T2>& rhs) noexcept {
        return &lhs == &rhs;
    }

    class LoggerReporter {
    public:

        static void reportAllocation(void* allocator, MemoryPool* pool, void* data, std::size_t bytes) {
            std::cerr << "Allocator(0x" << allocator << ") pool[0x" << pool << "] ptr[0x" << data << "] ::: Alloc " << bytes << " bytes." << std::endl;
        }

        static void reportDeallocation(void* allocator, MemoryPool* pool, void* data, std::size_t bytes) {
            std::cerr << "Allocator(0x" << allocator << ") pool[0x" << pool << "] ptr[0x" << data << "] ::: Free " << bytes << " bytes." << std::endl;
        }

        static void reportLeaks(void* allocator, MemoryPool* pool) {
            std::cerr << "Allocator(0x" << allocator << ") ::: leaks detected, losing: " << pool->bytes << " bytes!" << std::endl;
        }

        static void reportZeroSizeUndefinedBehavior(void* allocator) {
            std::cerr << "Allocator(0x" << allocator << ") ::: error detected, size is 0." << std::endl;
        }

        static void reportNoByteInPool(void* allocator) {
            std::cerr << "Allocator(0x" << allocator << ") ::: dealloc error detected, no more byte in the pool." << std::endl;
        }
    };

    template<class T>
    class LoggerAllocator: public Allocator<T, LoggerReporter> {};

}

