/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/ecs/world.hpp>
#include <netero/logger.hpp>

#include <gtest/gtest.h>

TEST(NeteroPatterns, entityLocalScope)
{
    netero::ecs::World world;

    LOG_INFO << "ecs_entity: Create entity first" << std::endl;
    netero::ecs::Entity first = world.createEntity();
    LOG_INFO << "ecs_entity: Create entity second" << std::endl;
    netero::ecs::Entity second = world.createEntity();
    LOG_INFO << "ecs_entity: Enable entity second" << std::endl;
    second.enable();
    LOG_INFO << "ecs_entity: entity killed" << std::endl;
    first.kill();
    EXPECT_FALSE(first.valid());
    LOG_INFO << "ecs_entity: update world" << std::endl;
    world.update();
    auto stat = world.getStatistic();
    EXPECT_EQ(stat.size, 1);
    LOG_INFO << "ecs_entity: update world" << std::endl;
    world.update();
    stat = world.getStatistic();
    EXPECT_EQ(stat.size, 1);
}
