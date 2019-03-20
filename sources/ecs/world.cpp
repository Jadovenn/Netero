/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>
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

	World::World(World &rhs) {
		_deleteEntities();
		_entities = rhs._entities;
	}

	World::World(World &&rhs) {
		_deleteEntities();
		_entities = rhs._entities;
		rhs._entities.clear();
	}
	
	const World	&World::operator=(const World& rhs) {
		_deleteEntities();
		_entities = rhs._entities;
		return *this;
	}

	World	&World::operator=(World&& rhs) {
		_deleteEntities();
		_entities = rhs._entities;
		return *this;
	}

	void	World::_deleteEntities() {
		for (auto *e : _entities) {
			delete e;
		}
		_entities.clear();
	}

	Entity	World::createEntity() {
		std::lock_guard<std::mutex>	lock(_entityAllocatorLock);
		auto newEntityContainer = new EntityContainer(this);
		_entities.push_back(newEntityContainer);
		Entity	newEntity(newEntityContainer);
		return newEntity;
	}

	Entity	World::createEntity(const std::string &name) {
		std::lock_guard<std::mutex>	lock(_entityAllocatorLock);
		auto newEntityContainer = new EntityContainer(this, name);
		_entities.push_back(newEntityContainer);
		Entity	newEntity(newEntityContainer);
		return newEntity;
	}

	void	World::killEntity(Entity &entity) {
		std::lock_guard<std::mutex>	lock(_entityAllocatorLock);
		auto entIt = std::find_if(_entities.begin(), _entities.end(), [&entity] (EntityContainer *ent) {
			return entity == ent;
		});
		if (entIt == _entities.end())
			return;
		entity.disable();
		entity.unregister();
		_localWorldCache.killedEntities.push_back(*entIt);
		_entities.erase(entIt);
	}

	std::size_t	World::size() {
		return _entities.size();
	}

	World::Statistic	&World::getStatistic() {
		_statistic.size = _entities.size();
		_statistic.activeEntities = _localWorldCache.activeEntities.size();
		_statistic.unactiveEntities = _localWorldCache.unactiveEntities.size();
		_statistic.garbadgeSize = _localWorldCache.killedEntities.size();
		return _statistic;
	}

	void	World::update() {
		if (_localWorldCache.statusFlag) {
			_localWorldCache.generate(_entities);
		}
		else {
			_localWorldCache.collectGarbage();
		}
	}

}
