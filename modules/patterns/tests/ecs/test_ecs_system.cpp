/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/logger.hpp>

#include "test_ecs_dataset.hpp"

#include <gtest/gtest.h>

namespace ecs = netero::ecs;

// this system will change position depending to the path of entities that contains a Path<int> and Position component
class PathSystem: public ecs::System<ecs::ComponentFilter<Path<int>, Position>> {
    public:
    ~PathSystem() override = default;
    void exec() final
    {
        for (
            auto entity :
            getActiveEntities()) { // getActiveEntities return only active entity that belong to this system
            auto &position = entity->getComponent<Position>();
            auto &path = entity->getComponent<Path<int>>();
            auto &name = entity->getComponent<
                Name>(); // unsafe because it is not specified in the componentFilter, so don't do that
            int next_x = path.points[path.idx].first;
            int next_y = path.points[path.idx].second;
            position.x = next_x;
            position.y = next_y;
            path.idx += 1;
            if (path.idx == path.size)
                path.idx = 0;
            LOG_INFO << name.name << " move to : " << position.x << ", " << position.y << std::endl;
        }
    }
};

class NameSystem: public ecs::System<ecs::ComponentFilter<Name>> {
    public:
    ~NameSystem() override = default;
    void exec() final
    {
        LOG_INFO << "enumerate entity with name:" << std::endl;
        for (auto entity : getActiveEntities()) {
            auto &name = entity->getComponent<Name>();
            LOG_INFO << name.name << std::endl;
        }
    }
};

TEST(NeteroPatterns, ecs_system)
{
    netero::ecs::World  world;
    netero::ecs::Entity first = world.createEntity();
    netero::ecs::Entity second = world.createEntity();
    netero::ecs::Entity third = world.createEntity();
    netero::ecs::Entity fourth = world.createEntity();
    netero::ecs::Entity fifth = world.createEntity();

    // Set up first entity
    first->addComponent<Position>(10, 4);
    first->addComponent<Name>("PNJ-1");
    first->addComponent<Path<int>>(10, 4, 12, 16, 18, 30);
    first.enable();

    // Set up second entity
    second->addComponent<Position>();
    second->addComponent<Name>("PNJ-2");
    second->addComponent<Path<int>>(0, 0, 5, 10, 10, 5, 10, 20);
    second.enable();

    // Set up third entity
    third->addComponent<Position>(40, 40);
    third->addComponent<Name>("Village panel");
    third->addComponent<Text>("Welcome to the village!");
    third.enable();

    // Set up fourth entity
    fourth->addComponent<Position>(100, 100);
    fourth->addComponent<Name>("House");
    fourth.enable();

    world.addSystem<NameSystem>();
    world.addSystem<PathSystem>();
    world.update();
    world.removeSystem<NameSystem>();
    world.update();
    world.update();
    world.update();
    world.removeSystem<PathSystem>();
    world.addSystem<NameSystem>();
    world.update();
}
