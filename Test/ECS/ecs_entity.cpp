/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include "../../Headers/netero/netero.hpp"

netero::ecs::World	world;

int	entityLocalScope() {
	std::cout << "ecs_entity: Create entity first" << std::endl;
	netero::ecs::Entity	first = world.createEntity();
	std::cout << "ecs_entity: entity killed" << std::endl;
	first.kill();
	if (first.valid())
		return 1;
	if (world.size() != 0)
		return 2;
}

int	main() {
	int ret = entityLocalScope();
	if (ret != 0)
		return ret;
	return 0;
}
