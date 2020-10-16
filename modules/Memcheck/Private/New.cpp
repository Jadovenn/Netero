/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <mutex>

#include <Netero/Debug.hpp>
#include <Netero/MemCheck/Allocator.hpp>
#include <Netero/MemCheck/Block.hpp>
#include <Netero/Netero.hpp>

Netero::Memck::MemBlockHeader* g_heap = nullptr;
static bool                    g_memcorruption_detection = false;
std::mutex                     g_allocMutex;

void* operator new(std::size_t size)
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    auto*                        header = static_cast<Netero::Memck::MemBlockHeader*>(malloc(
        size + sizeof(Netero::Memck::MemBlockHeader) + sizeof(Netero::Memck::MemBlockFooter)));
    header->magic = Netero::Memck::headerMagic;
    header->bytes = size;
    header->footer = reinterpret_cast<Netero::Memck::MemBlockFooter*>(
        (uintptr_t)header + sizeof(Netero::Memck::MemBlockHeader) + size);
    header->footer->magic = Netero::Memck::footerMagic;
    if (likely(g_heap)) {
        header->next = g_heap;
        header->prev = nullptr;
    }
    else {
        header->next = nullptr;
        header->prev = nullptr;
        g_heap = header;
    }
    void* ptr = reinterpret_cast<void*>((uintptr_t)header + sizeof(Netero::Memck::MemBlockHeader));
    //std::cout << "Allocator(global new) ::: Alloc data(0x" << ptr << ")" << std::endl;
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    if (!ptr) {
        return;
    };
    auto* header = reinterpret_cast<Netero::Memck::MemBlockHeader*>(
        (uintptr_t)ptr - sizeof(Netero::Memck::MemBlockHeader));
    if (g_memcorruption_detection && header->footer->magic != Netero::Memck::footerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr << ") ::: Corrupted." << std::endl;
        BREAK;
        return;
    }
    if (g_memcorruption_detection && header->magic != Netero::Memck::headerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr
                  << ") ::: wrong pointer, this block in not managed bu the heap." << std::endl;
        BREAK;
        return;
    }
    if (likely(header->next)) {
        header->next->prev = header->prev;
    }
    if (likely(header->prev)) {
        header->prev->next = header->next;
    }
    if (unlikely(!header->prev)) {
        g_heap = header->next;
    }
    if (unlikely(!header->prev && !header->next)) {
        g_heap = nullptr;
    }
    //std::cout << "Allocator(global new) ::: free data(0x" << ptr << ")" << std::endl;
    std::free(header);
}

void* operator new[](std::size_t size)
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    auto*                        header = static_cast<Netero::Memck::MemBlockHeader*>(malloc(
        size + sizeof(Netero::Memck::MemBlockHeader) + sizeof(Netero::Memck::MemBlockFooter)));
    header->magic = Netero::Memck::headerMagic;
    header->bytes = size;
    header->footer = reinterpret_cast<Netero::Memck::MemBlockFooter*>(
        (uintptr_t)header + sizeof(Netero::Memck::MemBlockHeader) + size);
    header->footer->magic = Netero::Memck::footerMagic;
    if (likely(g_heap)) {
        header->next = g_heap;
        header->prev = nullptr;
    }
    else {
        header->next = nullptr;
        header->prev = nullptr;
        g_heap = header;
    }
    void* ptr = reinterpret_cast<void*>((uintptr_t)header + sizeof(Netero::Memck::MemBlockHeader));
    //std::cout << "Allocator(global new) ::: Alloc data(0x" << ptr << ")" << std::endl;
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    if (!ptr) {
        return;
    };
    auto* header = reinterpret_cast<Netero::Memck::MemBlockHeader*>(
        (uintptr_t)ptr - sizeof(Netero::Memck::MemBlockHeader));
    if (g_memcorruption_detection && header->footer->magic != Netero::Memck::footerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr << ") ::: Corrupted." << std::endl;
        BREAK;
        return;
    }
    if (g_memcorruption_detection && header->magic != Netero::Memck::headerMagic) {
        std::cout << "Allocator(global new) data(0x" << ptr
                  << ") ::: wrong pointer, this block in not managed bu the heap." << std::endl;
        BREAK;
        return;
    }
    if (likely(header->next)) {
        header->next->prev = header->prev;
    }
    if (likely(header->prev)) {
        header->prev->next = header->next;
    }
    if (unlikely(!header->prev)) {
        g_heap = header->next;
    }
    if (unlikely(!header->prev && !header->next)) {
        g_heap = nullptr;
    }
    //std::cout << "Allocator(global new) ::: free data(0x" << ptr << ")" << std::endl;
    std::free(header);
}

namespace Netero::Memchk {

void ReportLeaks()
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    size_t                       nb_block = 0;
    size_t                       bytes = 0;
    for (auto* block = g_heap; block; block = g_heap->next) {
        bytes += block->bytes;
        nb_block += 1;
    }
    if (nb_block) {
        std::cout << "Allocator(global new) ::: " << nb_block << " blocks have leak for " << bytes
                  << " bytes." << std::endl;
    }
    else {
        std::cout << "Allocator(global new) ::: No blocks in the heap." << std::endl;
    }
}

void ActivateMemCorruptionDetection()
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    g_memcorruption_detection = true;
}

void DeactivateMemCorruptionDetection()
{
    std::scoped_lock<std::mutex> lock(g_allocMutex);
    g_memcorruption_detection = false;
}

} // namespace Netero::Memchk
