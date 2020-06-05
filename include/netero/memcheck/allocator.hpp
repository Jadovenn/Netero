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

    template<typename T>
    class Allocator {
    public:
        typedef T               value_type;
        typedef std::size_t     size_type;
        typedef std::ptrdiff_t  difference_type;
        typedef std::true_type  propagate_on_container_mode_assignement;
        typedef std::false_type is_always_equal;

        MemoryPool* pool;

        Allocator() {
            pool = reinterpret_cast<MemoryPool*>(std::malloc(sizeof(MemoryPool)));
            pool->bytes = 0;
            pool->owner = this;
        }

        template<typename U>
        Allocator(const Allocator<U>& other) {
            this->pool = other.pool;
        }

        ~Allocator() {
            if (this == pool->owner) {
                if (pool->bytes != 0) {
                    std::cerr << "Allocator(0x" << this << ") ::: leaks detected, losing: " << pool->bytes << " bytes!" << std::endl;
                }
                std::free(pool);
            }
        }

        [[nodiscard]] constexpr T* allocate(std::size_t size) {
            if (std::numeric_limits<std::size_t>::max() / sizeof(T) < size) {
                throw std::bad_array_new_length();
            }
            T* data = reinterpret_cast<T*>(std::malloc(sizeof(T) * size));
            if (!data) {
                throw std::bad_alloc();
            }
            std::cerr << "Allocator(0x" << this << ") pool[0x" << this->pool << "] ptr[0x" << data << "] ::: Alloc " << sizeof(T) * size << " bytes." << std::endl;
            pool->bytes += size * sizeof(T);
            return data;
        }

        constexpr void deallocate(T* data, std::size_t size) {
            if (size == 0) {
                std::cerr << "Allocator(0x" << this << ") ::: error detected, try do deallocate 0." << std::endl;
            }
            if (pool->bytes == 0) {
                std::cerr << "Allocator(0x" << this << ") ::: dealloc error detected, no more byte in the pool." << std::endl;
            }
            std::cerr << "Allocator(0x" << this << ") pool[0x" << this->pool << "] ptr[0x" << data << "] ::: Free " << sizeof(T) * size << " bytes." << std::endl;
            std::free(data);
            this->pool->bytes -= sizeof(T) * size;
        }

        template<typename U>
        struct rebind {
            typedef Allocator<U>    other;
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
}

