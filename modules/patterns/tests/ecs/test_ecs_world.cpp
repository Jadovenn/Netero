/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/ecs/world.hpp>
#include <netero/logger.hpp>

#include <gtest/gtest.h>

TEST(NeteroPatterns, createEntity)
{
    netero::ecs::World world;

    LOG_INFO << "ecs_world: Create entity first" << std::endl;
    netero::ecs::Entity first = world.createEntity();
    EXPECT_TRUE(first.valid());
    LOG_INFO << "ecs_world: Create entity second" << std::endl;
    netero::ecs::Entity second = world.createEntity("second");
    EXPECT_TRUE(second.valid());
    LOG_INFO << "ecs_world: World size " << world.size() << std::endl;
    EXPECT_TRUE(world.size() == 2);
    LOG_INFO << "ecs_world: Kill entity second" << std::endl;
    world.killEntity(first);
    EXPECT_FALSE(first.valid());
    LOG_INFO << "ecs_world: World size " << world.size() << std::endl;
    EXPECT_EQ(world.size(), 1);
}
