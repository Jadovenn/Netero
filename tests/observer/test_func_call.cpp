/**
* Netero sources under BSD-3-Clause
* see LICENCE.txt
*/

#include <netero/observer/slot.hpp>

int		add(int a, int b) {
	return a + b;
}

int	main() {
	netero::slot<int(int, int)>	slot(&add);
	if (slot(21, 21) != 42) {
		return 1;
	}
	return 0;
}
