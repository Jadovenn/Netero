/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "netero/set.hpp"

int 	main() {
	netero::set<int>	a{1, 2, 3, 4, 5, 6, 7, 8};
	netero::set<int>	b{6, 7, 8};
	std::set<int>		c{4, 5, 6};
	netero::set<int>	d(c);
	netero::set<int>	e{5, 10};

	if (b.isSubsetOf(c)) // NOT OK
		return 1;
	if (!b.isSubsetOf(a)) // OK
		return 1;
	if (e.isSubsetOf(a)) {
		return 1;
	}
	if (!e.interWith(a)) {
		return 1;
	}
	return 0;
}
