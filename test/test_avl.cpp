/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#define NETERO_DEBUG
#include "netero/avl.hpp"

int 	main() {
	netero::avl<int>		avl_tree;

	avl_tree.add(5);
	avl_tree.add(2);
	avl_tree.add(1);
	avl_tree.add(12);
	avl_tree.add(3);
	if (!avl_tree.search(1))
		return 1;
	if (avl_tree.search(24))
		return 1;
	avl_tree.display();
	for (const auto &e: avl_tree) {
		std::cout << e << std::endl;
	}
	return 0;
}
