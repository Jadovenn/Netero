/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

namespace netero::memck {

    constexpr uint32_t headerMagic = 0xDEADBEEF;
    constexpr uint32_t footerMagic = 0x1BADB002;

    struct MemBlockHeader {
        const uint32_t  magic = headerMagic;
    };

    struct MemBlockFooter {
        const uint32_t  magic = footerMagic;
    };

    struct Memblock {
        MemBlockHeader* header;
        void*           data;
        MemBlockFooter* footer;
    };

}

