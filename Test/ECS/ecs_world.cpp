/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "../../Headers/netero/netero.hpp"

netero::ecs::World	world;

int	createEntity() {
	netero::ecs::Entity	first = world.createEntity();
	if (!first.valid())
		return 1;
	netero::ecs::Entity second = world.createEntity("second");
	if (!second.valid())
		return 2;
	if (world.size() != 2)
		return 3;
	world.killEntity(first);
	if (first.valid())
		return 4;
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
