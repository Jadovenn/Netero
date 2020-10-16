/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <Netero/ECS/World.hpp>
#include <Netero/Logger.hpp>

#include <gtest/gtest.h>

TEST(NeteroPatterns, entityLocalScope)
{
    Netero::ECS::World world;

    LOG_INFO << "ecs_entity: Create entity first" << std::endl;
    Netero::ECS::Entity first = world.CreateEntity();
    LOG_INFO << "ecs_entity: Create entity second" << std::endl;
    Netero::ECS::Entity second = world.CreateEntity();
    LOG_INFO << "ecs_entity: Enable entity second" << std::endl;
    second.Enable();
    LOG_INFO << "ecs_entity: entity killed" << std::endl;
    first.Kill();
    EXPECT_FALSE(first.Valid());
    LOG_INFO << "ecs_entity: update world" << std::endl;
    world.Update();
    auto stat = world.GetStatistic();
    EXPECT_EQ(stat.size, 1);
    LOG_INFO << "ecs_entity: update world" << std::endl;
    world.Update();
    stat = world.GetStatistic();
    EXPECT_EQ(stat.size, 1);
}
