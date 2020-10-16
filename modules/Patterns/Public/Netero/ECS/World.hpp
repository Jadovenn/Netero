/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <exception>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <Netero/ECS/Entity.hpp>
#include <Netero/ECS/System.hpp>
#include <Netero/TypeId.hpp>

/**
 * Process to update the world
 * 1 if the status flag is true generate the entity cache for system computation
 * 2 execute each system (they use their own cache so no search needed on entity's world stack)
 * 3 each system make a boolean check on the status of the entity
 * 4 the cache collect entity to be deleted, so delete this entity
 * 5 if the status flag is true clean up the cache
 */

namespace Netero::ECS {

class World {
    public:
    struct Statistic {
        size_t size;
        size_t activeEntities;
        size_t unactiveEntities;
        size_t garbadgeSize;
    };

    public:
    World();
    ~World();
    World(World &rhs) = delete;
    World(World &&rhs) = delete;
    const World &operator=(const World &rhs) = delete;
    World &      operator=(World &&rhs) = delete;

    Entity CreateEntity();
    Entity CreateEntity(const std::string &name);
    void   KillEntity(Entity &entity);
    void   EnableEntity(Entity &entity);
    void   DisableEntity(Entity &entity);

    template<typename T, typename... Args>
    void AddSystem(Args... args)
    {
        static_assert(
            std::is_base_of<BaseSystem, T>::value,
            "System not base on BaseSystem, your system must inherit from netero::system<>");
        T *data = new (std::nothrow) T(std::forward(args)...);
        if (!data)
            throw std::bad_alloc();
        _systems[Netero::TypeID<BaseSystem>::GetTypeID<T>()] = data;
    }

    template<typename T>
    void RemoveSystem()
    {
        static_assert(
            std::is_base_of<BaseSystem, T>::value,
            "System not base on BaseSystem, your system must inherit from netero::system<>");
        auto systemIt = _systems.find(Netero::TypeID<BaseSystem>::GetTypeID<T>());
        if (systemIt == _systems.end())
            return;
        delete (*systemIt).second;
        _systems.erase(systemIt);
    }

    std::size_t       Size();
    World::Statistic &GetStatistic();

    void Update();

    private:
    void                                    _generateCache();
    void                                    _deleteEntities();
    std::mutex                              _entityAllocatorLock;
    Netero::Set<EntityContainer *>          _entitiesEnable;
    Netero::Set<EntityContainer *>          _entitiesDisable;
    std::map<Netero::type_id, BaseSystem *> _systems;
    World::Statistic                        _statistic;
};

} // namespace Netero::ECS
