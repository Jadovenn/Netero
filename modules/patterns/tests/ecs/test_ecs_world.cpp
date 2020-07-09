/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <cassert>
#include <iostream>

#include <netero/ecs/world.hpp>

netero::ecs::World world;

void createEntity()
{
    std::cout << "ecs_world: Create entity first" << std::endl;
    netero::ecs::Entity first = world.createEntity();
    assert(first.valid());
    std::cout << "ecs_world: Create entity second" << std::endl;
    netero::ecs::Entity second = world.createEntity("second");
    assert(second.valid());
    std::cout << "ecs_world: World size " << world.size() << std::endl;
    assert(world.size() == 2);
    std::cout << "ecs_world: Kill entity second" << std::endl;
    world.killEntity(first);
    assert(!first.valid());
    std::cout << "ecs_world: World size " << world.size() << std::endl;
    assert(world.size() == 1);
}

int main()
{
    createEntity();
    return 0;
}
