/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "netero/ECS/world.hpp"

namespace netero::ecs {

	World::World() {

	}

	World::~World() {
		_deleteEntities();
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

}

