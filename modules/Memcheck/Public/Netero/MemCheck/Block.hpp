/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstdint>

namespace Netero::Memck {

constexpr uint32_t headerMagic = 0xDEADBEEF;
constexpr uint32_t footerMagic = 0x1BADB002;

struct MemBlockFooter {
    uint32_t magic;
};

struct MemBlockHeader {
    uint32_t               magic;
    size_t                 bytes;
    struct MemBlockHeader* next;
    struct MemBlockHeader* prev;
    MemBlockFooter*        footer;
};

} // namespace Netero::Memck
