/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <netero/ecs/world.hpp>

netero::ecs::World	world;

int	createEntity() {
	std::cout << "ecs_world: Create entity first" << std::endl;
	netero::ecs::Entity	first = world.createEntity();
	if (!first.valid())
		return 1;
	std::cout << "ecs_world: Create entity second" << std::endl;
	netero::ecs::Entity second = world.createEntity("second");
	if (!second.valid())
		return 2;
	std::cout << "ecs_world: World size " << world.size() << std::endl;
	if (world.size() != 2)
		return 3;
	std::cout << "ecs_world: Kill entity second" << std::endl;
	world.killEntity(first);
	if (first.valid())
		return 4;
	std::cout << "ecs_world: World size " << world.size() << std::endl;
	if (world.size() != 1)
		return 5;
	return 0;
}

int	main() {
	int ret = createEntity();
	if (ret != 0)
		return ret;
	return 0;
}
