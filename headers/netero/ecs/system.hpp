/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <type_traits>
#include <vector>
#include <map>
#include "entity.hpp"
#include "component_filter.hpp"
#include "netero/set.hpp"

namespace netero {
	namespace ecs {

		class World;

		struct SystemCache {
			std::map<EntityContainer*, Entity>		_localEntities;
		};

		class BaseSystem
		{
			friend World;
		public:
			BaseSystem(const netero::set<netero::type_id> &includeFilterSet,
						const netero::set<netero::type_id> &excludeFilterSet)
					: _includeFilterSet(includeFilterSet),
					  _excludeFilterSet(excludeFilterSet)
			{}
			virtual ~BaseSystem() = default;

		private:
			void	registerEntities(EntityContainer	*entity) {
				if (!entity)
					return;
				_localSystemCache._localEntities[entity] = Entity(entity);
			}

			void 	unregisterEntities(EntityContainer *entity) {
				if (!entity)
					return;
				_localSystemCache._localEntities.erase(entity);
			}

		public:
			virtual void	exec() = 0;
			const netero::set<netero::type_id>	&_includeFilterSet;
			const netero::set<netero::type_id>	&_excludeFilterSet;
		protected:
			SystemCache		_localSystemCache;
		};

		template<typename IncludeComponent = ComponentFilter<>, typename ExcludeComponent = ComponentFilter<>,
				 typename = std::enable_if<std::is_base_of<IncludeComponent, BaseComponentFilter>::value>,
				 typename = std::enable_if<std::is_base_of<ExcludeComponent, BaseComponentFilter>::value>>
		class System : public BaseSystem {
		public:
			System()
				: BaseSystem(IncludeComponent::getFilter(), ExcludeComponent::getFilter())
				{}
			virtual ~System() = default;

			const std::map<EntityContainer*, Entity>	&getEntities() {
				return _localSystemCache._localEntities;
			}
		};
	}
}
