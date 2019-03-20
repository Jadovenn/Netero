/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>
#include "netero/netero.hpp"

struct CompA: public netero::ecs::Component {
	int	data;
};

struct CompB : public netero::ecs::Component {
	char data;
};

using	GroupAB = netero::ecs::ComponentFilter<CompA, CompB>;
using	GroupA = netero::ecs::ComponentFilter<CompA>;
using	GroupB = netero::ecs::ComponentFilter<CompB>;

int	main() {
	std::list<std::size_t>	types = GroupAB::getTypes();
	if (std::binary_search(types.begin(), types.end(), 0) != true)
		return 1;
	if (std::binary_search(types.begin(), types.end(), 1) != true)
		return 2;
	types = GroupA::getTypes();
	if (std::binary_search(types.begin(), types.end(), 0) != true)
		return 3;
	if (std::binary_search(types.begin(), types.end(), 1) == true)
		return 4;
	types = GroupB::getTypes();
	if (std::binary_search(types.begin(), types.end(), 0) == true)
		return 5;
	if (std::binary_search(types.begin(), types.end(), 1) != true)
		return 6;
	return 0;
}
