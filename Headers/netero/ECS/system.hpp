/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <type_traits>
#include <vector>
#include "entity.hpp"
#include "component_filter.hpp"

namespace netero {
	namespace ecs {

		class World;

		struct SystemCache {

			void				addEntity(EntityContainer *entity);
			void				removeEntity(EntityContainer *entity);
			std::vector<Entity>		_localEntities;
		};

		class BaseSystem
		{
			friend World;
		public:
			virtual ~BaseSystem() = default;

			virtual void	exec() = 0;
			virtual void	updateEntitiesCache(std::vector<EntityContainer*> &entities) = 0;
		protected:
			SystemCache		_localSystemCache;
		};

		template<typename IncludeComponent = ComponentFilter<>, typename ExcludeComponent = ComponentFilter<>,
				 typename = std::enable_if<std::is_base_of<IncludeComponent, BaseComponentFilter>::value>,
				 typename = std::enable_if<std::is_base_of<ExcludeComponent, BaseComponentFilter>::value>>
		class System : public BaseSystem {
		public:
			System() {
			}
			virtual ~System() = default;

			void	updateEntitiesCache(std::vector<EntityContainer*> &entities) final {

			}

		protected:

		private:	

			netero::type_id				_includeList;
			netero::type_id				_excludeList;
		};
	}
}
