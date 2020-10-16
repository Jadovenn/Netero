/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>

#include <Netero/ECS/World.hpp>

namespace Netero::ECS {

World::World()
{
}

World::~World()
{
    _deleteEntities();
    for (auto &system : _systems) {
        delete system.second;
    }
}

void World::_deleteEntities()
{
    for (auto *e : _entitiesDisable) {
        delete e;
    }
    for (auto *e : _entitiesEnable) {
        delete e;
    }
    _entitiesDisable.clear();
    _entitiesEnable.clear();
}

Entity World::CreateEntity()
{
    std::lock_guard<std::mutex> lock(_entityAllocatorLock);
    auto                        newEntityContainer = new EntityContainer(this);
    _entitiesDisable.insert(newEntityContainer);
    Entity newEntity(newEntityContainer);
    return newEntity;
}

Entity World::CreateEntity(const std::string &name)
{
    std::lock_guard<std::mutex> lock(_entityAllocatorLock);
    auto                        newEntityContainer = new EntityContainer(this, name);
    _entitiesDisable.insert(newEntityContainer);
    return Entity(newEntityContainer);
}

void World::KillEntity(Entity &entity)
{
    std::lock_guard<std::mutex> lock(_entityAllocatorLock);
    entity.Disable();
    auto entIt = std::find_if(_entitiesDisable.begin(),
                              _entitiesDisable.end(),
                              [&entity](EntityContainer *ent) { return entity == ent; });
    if (entIt == _entitiesDisable.end())
        return;
    delete *entIt;
    _entitiesDisable.erase(*entIt);
    entity.Unregister();
}

void World::EnableEntity(Netero::ECS::Entity &entity)
{
    auto findIt = _entitiesDisable.find(
        entity.operator->()); // TODO : create method to access the container properly
    if (findIt == _entitiesDisable.end())
        return;
    _entitiesEnable.insert(*findIt);
    _entitiesDisable.erase(*findIt);
    entity->status = true;
}

void World::DisableEntity(Netero::ECS::Entity &entity)
{
    auto findIt = _entitiesEnable.find(
        entity.operator->()); // TODO : create method to access the container properly
    if (findIt == _entitiesEnable.end())
        return;
    _entitiesDisable.insert(*findIt);
    _entitiesEnable.erase(*findIt);
    entity->status = false;
}

std::size_t World::Size()
{
    return _entitiesDisable.size();
}

World::Statistic &World::GetStatistic()
{
    _statistic.size = _entitiesEnable.size();
    return _statistic;
}

void World::_generateCache()
{
    for (auto &system : _systems) {
        system.second->generateCache(_entitiesEnable);
    }
}

void World::Update()
{
    _generateCache();
    for (auto &system : _systems) {
        system.second->exec();
    }
}

} // namespace Netero::ECS
