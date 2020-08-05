/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/ecs/world.hpp>
#include <netero/logger.hpp>

#include <gtest/gtest.h>

netero::ecs::World world;

struct Position: public netero::ecs::Component {
    Position(int x = 0, int y = 0): x(x), y(y) {}
    int x;
    int y;
};

struct Id: public netero::ecs::Component {
    std::string firstName;
    std::string lastName;
};

TEST(NeteroPatterns, component)
{
    LOG_INFO << "ecs_entity: Create entity first" << std::endl;
    netero::ecs::Entity first = world.createEntity();
    LOG_INFO << "ecs_entity: Add Component Position to Entity first" << std::endl;
    auto &position = first->addComponent<Position>(10, 9);
    auto &id = first->addComponent<Id>();
    id.firstName = "test";
    EXPECT_THROW(first->addComponent<Position>(), std::runtime_error);
    EXPECT_NO_THROW({
        auto &pos = first->getComponent<Position>();
        auto &i = first->getComponent<Id>();
        EXPECT_EQ(pos.x, 10);
        EXPECT_EQ(i.firstName, id.firstName);
    });
    EXPECT_NO_THROW(first->deleteComponent<Id>());
}
