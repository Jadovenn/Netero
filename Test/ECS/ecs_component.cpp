/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include "../../Headers/netero/netero.hpp"

netero::ecs::World	world;

struct Position : public netero::ecs::Component {
	Position(int x = 0, int y = 0)
		:	x(y), y(y)
	{}
	int		x;
	int		y;
};

int	component() {
	std::cout << "ecs_entity: Create entity first" << std::endl;
	netero::ecs::Entity	first = world.createEntity();
	std::cout << "ecs_entity: Add Component Position to Entity first" << std::endl;
	auto &position = first->addComponent<Position>(10, 9);
	try {
		first->addComponent<Position>();
		return 1;
	}
	catch (...) {
	}
	return 0;
}

int	main() {
	int ret = component();
	if (ret != 0)
		return ret;
	return 0;
}