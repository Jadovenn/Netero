/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/buffer.hpp>

int		main() {
    int     tmp[5];
    netero::shared_buffer<int>   buffer(0);

    if (buffer.getPadding() != 0) {
        return 1;
    }
    if (buffer.read(tmp, 5) != 0) {
        return 1;
    }
	return 0;
}

