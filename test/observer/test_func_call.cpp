/**
 *  Created by Jean-Adrien DOMAGE on 12/12/2019.
 */

#include "netero/observer/slot.hpp"

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
