/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstdint>

namespace netero::memck {

    constexpr uint32_t headerMagic = 0xDEADBEEF;
    constexpr uint32_t footerMagic = 0x1BADB002;

    struct MemBlockFooter {
        uint32_t  magic;
    };

    struct MemBlockHeader {
        uint32_t  magic;
        MemBlockFooter* footer;
    };
}

