/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

// add debug info to netero, enable display method
#define NETERO_DEBUG

#include "netero/avl.hpp"

int 	main() {
	netero::avl<int>		avl_tree;

	avl_tree.add(5);
	avl_tree.add(2);
	avl_tree.add(1);
	avl_tree.add(12);
	avl_tree.add(24);
	avl_tree.add(3);
	avl_tree.add(4);
	if (!avl_tree.search(1))
		return 1;
	if (avl_tree.search(23))
		return 1;
	avl_tree.display();
	for (const auto &e: avl_tree) {
		std::cout << e << std::endl;
	}
	avl_tree.display();
	return 0;
}
