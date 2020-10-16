/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <Netero/ECS/World.hpp>
#include <Netero/Logger.hpp>

#include <gtest/gtest.h>

Netero::ECS::World world;

struct Position: public Netero::ECS::Component {
    Position(int x = 0, int y = 0): x(x), y(y) {}
    int x;
    int y;
};

struct Id: public Netero::ECS::Component {
    std::string firstName;
    std::string lastName;
};

TEST(NeteroPatterns, component)
{
    LOG_INFO << "ecs_entity: Create entity first" << std::endl;
    Netero::ECS::Entity first = world.CreateEntity();
    LOG_INFO << "ecs_entity: Add Component Position to Entity first" << std::endl;
    auto &position = first->AddComponent<Position>(10, 9);
    auto &id = first->AddComponent<Id>();
    id.firstName = "test";
    EXPECT_THROW(first->AddComponent<Position>(), std::runtime_error);
    EXPECT_NO_THROW({
        auto &pos = first->GetComponent<Position>();
        auto &i = first->GetComponent<Id>();
        EXPECT_EQ(pos.x, 10);
        EXPECT_EQ(i.firstName, id.firstName);
    });
    EXPECT_NO_THROW(first->DeleteComponent<Id>());
}
