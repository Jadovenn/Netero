/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/avl.hpp>
#include <netero/memcheck/block.hpp>

namespace netero::memck {
    netero::avl<uint32_t>   heap;
}

