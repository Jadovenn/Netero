/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>
#include <iostream>
#include "netero/ecs/world.hpp"

namespace netero::ecs {

	World::World() {

	}

	World::~World() {
		_deleteEntities();
		for (auto &system: _systems) {
			delete system.second;
		}
	}

	void	World::_deleteEntities() {
		for (auto *e : _entitiesDisable) {
			delete e;
		}
		for (auto *e : _entitiesEnable) {
			delete e;
		}
		_entitiesDisable.clear();
		_entitiesEnable.clear();
	}

	Entity	World::createEntity() {
		std::lock_guard<std::mutex>	lock(_entityAllocatorLock);
		auto newEntityContainer = new EntityContainer(this);
		_entitiesDisable.insert(newEntityContainer);
		Entity	newEntity(newEntityContainer);
		return newEntity;
	}

	Entity	World::createEntity(const std::string &name) {
		std::lock_guard<std::mutex>	lock(_entityAllocatorLock);
		auto newEntityContainer = new EntityContainer(this, name);
		_entitiesDisable.insert(newEntityContainer);
		Entity	newEntity(newEntityContainer);
		return newEntity;
	}

	void	World::killEntity(Entity &entity) {
		std::lock_guard<std::mutex>	lock(_entityAllocatorLock);
		entity.disable();
		auto entIt = std::find_if(_entitiesDisable.begin(), _entitiesDisable.end(), [&entity] (EntityContainer *ent) {
			return entity == ent;
		});
		if (entIt == _entitiesDisable.end())
			return;
		delete *entIt;
		_entitiesDisable.erase(*entIt);
		entity.unregister();
	}

	void 	World::enableEntity(netero::ecs::Entity &entity) {
		auto findIt = _entitiesDisable.find(entity.operator->()); // TODO : create method to access the container properly
		if (findIt == _entitiesDisable.end())
			return;
		_entitiesEnable.insert(*findIt);
		_entitiesDisable.erase(*findIt);
		entity->status = true;
	}

	void 	World::disableEntity(netero::ecs::Entity &entity) {
		auto findIt = _entitiesEnable.find(entity.operator->()); // TODO : create method to access the container properly
		if (findIt == _entitiesEnable.end())
			return;
		_entitiesDisable.insert(*findIt);
		_entitiesEnable.erase(*findIt);
		entity->status = false;
	}

	std::size_t	World::size() {
		return _entitiesDisable.size();
	}

	World::Statistic	&World::getStatistic() {
		_statistic.size = _entitiesEnable.size();
		return _statistic;
	}

	void 	World::_generateCache() {
	    for(auto &system: _systems) {
	        system.second->generateCache(_entitiesEnable);
	    }
	}

	void	World::update() {
		_generateCache();
	    for(auto &system: _systems) {
	        system.second->exec();
	    }
	}

}
