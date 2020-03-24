/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <mutex>
#include <memory>
#include <exception>
#include <string>
#include <map>
#include <vector>
#include <netero/core/type_id.hpp>
#include <netero/ecs/entity.hpp>
#include <netero/ecs/system.hpp>

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
			World(World &rhs) = delete;
			World(World &&rhs) = delete;
			const World	&operator=(const World& rhs) = delete;
			World	&operator=(World&& rhs) = delete;

			Entity	createEntity();
			Entity	createEntity(const std::string &name);
			void	killEntity(Entity &entity);
			void 	enableEntity(Entity &entity);
			void 	disableEntity(Entity &entity);
			
			template<typename T, typename ...Args>
			void	addSystem(Args ...args) {
				static_assert(std::is_base_of<BaseSystem, T>::value, "System not base on BaseSystem, your system must inherit from netero::system<>");
				T	*data = new (std::nothrow) T(std::forward(args)...);
				if (!data)
					throw std::bad_alloc();
				_systems[netero::TypeID<BaseSystem>::getTypeID<T>()] = data;
			}

			template<typename T>
			void	removeSystem() {
				static_assert(std::is_base_of<BaseSystem, T>::value, "System not base on BaseSystem, your system must inherit from netero::system<>");
				auto systemIt = _systems.find(netero::TypeID<BaseSystem>::getTypeID<T>());
				if (systemIt == _systems.end())
					return;
				delete (*systemIt).second;
				_systems.erase(systemIt);
			}

			std::size_t	size();
			World::Statistic	&getStatistic();

			void	update();
		private:
			void 	_generateCache();
			void	_deleteEntities();
			std::mutex						            _entityAllocatorLock;
			netero::set<EntityContainer*>	            _entitiesEnable;
			netero::set<EntityContainer*>	            _entitiesDisable;
			std::map<netero::type_id, BaseSystem*>		_systems;
			World::Statistic				            _statistic;
		};

	}
}

