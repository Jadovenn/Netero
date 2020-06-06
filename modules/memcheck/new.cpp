/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <mutex>
#include <netero/avl.hpp>
#include <netero/memcheck/allocator.hpp>
#include <netero/memcheck/block.hpp>

netero::avl<netero::memck::MemBlockHeader*, netero::memck::Allocator<netero::memck::MemBlockHeader*>> heap;
std::mutex  allocMutex;

void* operator new(std::size_t size) {
    std::scoped_lock<std::mutex>    lock(allocMutex);
    auto* header = static_cast<netero::memck::MemBlockHeader*>(malloc(size + sizeof(netero::memck::MemBlockHeader) + sizeof(netero::memck::MemBlockFooter)));
    header->magic = netero::memck::headerMagic;
    header->footer = reinterpret_cast<netero::memck::MemBlockFooter*>((uintptr_t)header + sizeof(netero::memck::MemBlockHeader) + size);
    header->footer->magic = netero::memck::footerMagic;
    heap.add(header);
    void* ptr = reinterpret_cast<void*>((uintptr_t)header + sizeof(netero::memck::MemBlockHeader));
    std::cout << "Allocator(global new) ::: Alloc data(0x" << ptr << ")" << std::endl;
    return ptr;
}

void operator delete(void *ptr) noexcept {
    std::scoped_lock<std::mutex>    lock(allocMutex);
    if (!ptr) { return; };
    auto* header = reinterpret_cast<netero::memck::MemBlockHeader*>((uintptr_t)ptr - sizeof(netero::memck::MemBlockHeader));
    if (header->footer->magic != netero::memck::footerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr << ") ::: Corrupted." << std::endl;
    }
    if (header->magic != netero::memck::headerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr << ") ::: wrong pointer, this block in not managed bu the heap." << std::endl;
        return;
    }
    heap.remove(header);
    std::cout << "Allocator(global new) ::: free data(0x" << ptr << ")" << std::endl;
    std::free(header);
}

void* operator new[](std::size_t size) {
    std::scoped_lock<std::mutex>    lock(allocMutex);
    auto* header = static_cast<netero::memck::MemBlockHeader*>(malloc(size + sizeof(netero::memck::MemBlockHeader) + sizeof(netero::memck::MemBlockFooter)));
    header->magic = netero::memck::headerMagic;
    header->footer = reinterpret_cast<netero::memck::MemBlockFooter*>((uintptr_t)header + sizeof(netero::memck::MemBlockHeader) + size);
    header->footer->magic = netero::memck::footerMagic;
    heap.add(header);
    void* ptr = reinterpret_cast<void*>((uintptr_t)header + sizeof(netero::memck::MemBlockHeader));
    std::cout << "Allocator(global new) ::: Alloc data(0x" << ptr << ")" << std::endl;
    return ptr;
}

void operator delete[](void *ptr) noexcept {
    std::scoped_lock<std::mutex>    lock(allocMutex);
    if (!ptr) { return; };
    auto* header = reinterpret_cast<netero::memck::MemBlockHeader*>((uintptr_t)ptr - sizeof(netero::memck::MemBlockHeader));
    if (header->footer->magic != netero::memck::footerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr << ") ::: Corrupted." << std::endl;
    }
    if (header->magic != netero::memck::headerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr << ") ::: wrong pointer, this block in not managed bu the heap." << std::endl;
        return;
    }
    heap.remove(header);
    std::cout << "Allocator(global new) ::: free data(0x" << ptr << ")" << std::endl;
    std::free(header);
}


