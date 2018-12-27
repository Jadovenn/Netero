/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include "netero/ECS/entity.hpp"

namespace netero::ecs {

	class World {
	public:
		World();
		~World();
		World(World &rhs);
		World(World &&rhs);
		const World	&operator=(const World& rhs);
		World	&operator=(World&& rhs);

		Entity	createEntity();
		Entity	createEntity(const std::string &name);
	private:
		void	_deleteEntities();
		std::mutex						_entityAllocatorLock;
		std::vector<EntityContainer*>	_entities;
	};
	
}
