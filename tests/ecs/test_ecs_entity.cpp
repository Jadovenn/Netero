/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include "netero/netero.hpp"

netero::ecs::World	world;

int	entityLocalScope() {
	std::cout << "ecs_entity: Create entity first" << std::endl;
	netero::ecs::Entity	first = world.createEntity();
	std::cout << "ecs_entity: Create entity second" << std::endl;
	netero::ecs::Entity	second = world.createEntity();
	std::cout << "ecs_entity: Enable entity second" << std::endl;
	second.enable();
	std::cout << "ecs_entity: entity killed" << std::endl;
	first.kill();
	if (first.valid())
		return 1;
	std::cout << "ecs_entity: update world" << std::endl;
	world.update();
	auto stat = world.getStatistic();
	if (stat.size != 1)
		return 2;
	std::cout << "ecs_entity: update world" << std::endl;
	world.update();
	stat = world.getStatistic();
	if (stat.size != 1)
		return 5;
	return 0;
}

int	main() {
	int ret = entityLocalScope();
	if (ret != 0)
		return ret;
	return 0;
}
