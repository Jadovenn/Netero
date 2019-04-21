/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <type_traits>
#include <vector>
#include "entity.hpp"
#include "component_filter.hpp"
#include "netero/set.hpp"

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
		protected:
			SystemCache		_localSystemCache;
		};

		template<typename IncludeComponent = ComponentFilter<>, typename ExcludeComponent = ComponentFilter<>,
				 typename = std::enable_if<std::is_base_of<IncludeComponent, BaseComponentFilter>::value>,
				 typename = std::enable_if<std::is_base_of<ExcludeComponent, BaseComponentFilter>::value>>
		class System : public BaseSystem {
		public:
			System()
				: _includeFilterSet(IncludeComponent::getFilter()),
				  _excludeFilterSet(ExcludeComponent::getFilter())
				{}
			virtual ~System() = default;

			void	updateEntitiesCache(Entity	&entities) {
			}

			const std::vector<Entity>	&getEntities() {
				return _localSystemCache._localEntities;
			}

		private:
			const netero::set<netero::type_id>	&_includeFilterSet;
			const netero::set<netero::type_id>	&_excludeFilterSet;
		};
	}
}
