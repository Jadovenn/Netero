/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <netero/buffer.hpp>

int		main() {
    int     tmp[5];
    netero::shared_buffer<int>   buffer(0);

    assert(buffer.getPadding() == 0);
    assert(buffer.read(tmp, 5) == 0);
	return 0;
}

