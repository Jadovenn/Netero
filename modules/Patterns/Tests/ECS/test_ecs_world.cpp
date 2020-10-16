/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <Netero/ECS/World.hpp>
#include <Netero/Logger.hpp>

#include <gtest/gtest.h>

TEST(NeteroPatterns, createEntity)
{
    Netero::ECS::World world;

    LOG_INFO << "ecs_world: Create entity first" << std::endl;
    Netero::ECS::Entity first = world.CreateEntity();
    EXPECT_TRUE(first.Valid());
    LOG_INFO << "ecs_world: Create entity second" << std::endl;
    Netero::ECS::Entity second = world.CreateEntity("second");
    EXPECT_TRUE(second.Valid());
    LOG_INFO << "ecs_world: World size " << world.Size() << std::endl;
    EXPECT_TRUE(world.Size() == 2);
    LOG_INFO << "ecs_world: Kill entity second" << std::endl;
    world.KillEntity(first);
    EXPECT_FALSE(first.Valid());
    LOG_INFO << "ecs_world: World size " << world.Size() << std::endl;
    EXPECT_EQ(world.Size(), 1);
}
