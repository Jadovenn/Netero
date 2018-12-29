/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include "entity.hpp"

/**
 * Process to update the world
 * 1 if the status flag is true generate the entity cache for system computation
 * 2 execute each system (they use their own cache so no search needed on entity's world stak)
 * 3 each system make a boolean check on the status of the entity
 * 4 the cache collect entity to be deleted, so delete this entity
 * 5 if the status flag is true clean up the cache
 */

namespace netero {
	namespace ecs {

		class World {
			struct Cache {
				Cache();

				void	tick();
				void	generate(std::vector<EntityContainer*> &entities);
				void	collectGarbage();
				bool							statusFlag;
				std::vector<EntityContainer*>	activeEntities;
				std::vector<EntityContainer*>	unactiveEntities;
				std::vector<EntityContainer*>	killedEntities;
			};

		public:
			struct Statistic {
				size_t	size;
				size_t  activeEntities;
				size_t  unactiveEntities;
				size_t	garbadgeSize;
			};
		public:
			World();
			~World();
			World(World &rhs);
			World(World &&rhs);
			const World	&operator=(const World& rhs);
			World	&operator=(World&& rhs);

			Entity	createEntity();
			Entity	createEntity(const std::string &name);
			void	killEntity(Entity &entity);
			std::size_t	size();
			World::Statistic	&getStatistic();

			void	update();
		private:
			void	_deleteEntities();
			std::mutex						_entityAllocatorLock;
			std::vector<EntityContainer*>	_entities;
			World::Cache					_localWorldCache;
			World::Statistic				_statistic;
		};

	}
}
