/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <Netero/Logger.hpp>

#include "test_ecs_dataset.hpp"

#include <gtest/gtest.h>

namespace ECS = Netero::ECS;

// this system will change position depending to the path of entities that contains a Path<int> and Position component
class PathSystem: public ECS::System<ECS::ComponentFilter<Path<int>, Position>> {
    public:
    ~PathSystem() override = default;
    void exec() final
    {
        // getActiveEntities return only active entity that belong to this system
        for (auto entity : GetActiveEntities()) {
            auto &position = entity->GetComponent<Position>();
            auto &path = entity->GetComponent<Path<int>>();
            // unsafe because it is not specified in the componentFilter, so don't do that
            auto &name = entity->GetComponent<Name>();
            int   next_x = path.points[path.idx].first;
            int   next_y = path.points[path.idx].second;
            position.x = next_x;
            position.y = next_y;
            path.idx += 1;
            if (path.idx == path.size)
                path.idx = 0;
            LOG_INFO << name.name << " move to : " << position.x << ", " << position.y << std::endl;
        }
    }
};

class NameSystem: public ECS::System<ECS::ComponentFilter<Name>> {
    public:
    ~NameSystem() override = default;
    void exec() final
    {
        LOG_INFO << "enumerate entity with name:" << std::endl;
        for (auto entity : GetActiveEntities()) {
            auto &name = entity->GetComponent<Name>();
            LOG_INFO << name.name << std::endl;
        }
    }
};

TEST(NeteroPatterns, ECS_system)
{
    Netero::ECS::World  world;
    Netero::ECS::Entity first = world.CreateEntity();
    Netero::ECS::Entity second = world.CreateEntity();
    Netero::ECS::Entity third = world.CreateEntity();
    Netero::ECS::Entity fourth = world.CreateEntity();
    Netero::ECS::Entity fifth = world.CreateEntity();

    // Set up first entity
    first->AddComponent<Position>(10, 4);
    first->AddComponent<Name>("PNJ-1");
    first->AddComponent<Path<int>>(10, 4, 12, 16, 18, 30);
    first.Enable();

    // Set up second entity
    second->AddComponent<Position>();
    second->AddComponent<Name>("PNJ-2");
    second->AddComponent<Path<int>>(0, 0, 5, 10, 10, 5, 10, 20);
    second.Enable();

    // Set up third entity
    third->AddComponent<Position>(40, 40);
    third->AddComponent<Name>("Village panel");
    third->AddComponent<Text>("Welcome to the village!");
    third.Enable();

    // Set up fourth entity
    fourth->AddComponent<Position>(100, 100);
    fourth->AddComponent<Name>("House");
    fourth.Enable();

    world.AddSystem<NameSystem>();
    world.AddSystem<PathSystem>();
    world.Update();
    world.RemoveSystem<NameSystem>();
    world.Update();
    world.Update();
    world.Update();
    world.RemoveSystem<PathSystem>();
    world.AddSystem<NameSystem>();
    world.Update();
}
