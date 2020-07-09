/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/ecs/world.hpp>

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

int component()
{
    std::cout << "ecs_entity: Create entity first" << std::endl;
    netero::ecs::Entity first = world.createEntity();
    std::cout << "ecs_entity: Add Component Position to Entity first" << std::endl;
    auto &position = first->addComponent<Position>(10, 9);
    auto &id = first->addComponent<Id>();
    id.firstName = "test";
    try {
        first->addComponent<Position>();
        return 1;
    }
    catch (...) {
    }
    try {
        auto &pos = first->getComponent<Position>();
        auto &i = first->getComponent<Id>();
        if (pos.x != 10)
            return 2;
        if (i.firstName != id.firstName)
            return 3;
    }
    catch (...) {
        return 4;
    }
    try {
        first->deleteComponent<Id>();
    }
    catch (...) {
        return 5;
    }
    return 0;
}

int main()
{
    int ret = component();
    if (ret != 0)
        return ret;
    return 0;
}