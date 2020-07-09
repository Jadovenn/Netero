/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <map>
#include <type_traits>
#include <vector>

#include <netero/ecs/component_filter.hpp>
#include <netero/ecs/entity.hpp>
#include <netero/set.hpp>

namespace netero {
namespace ecs {

    class World;

    struct SystemCache {
        std::vector<Entity> _unactiveEntities;
        std::vector<Entity> _activeEntities;
        void                flush()
        {
            _unactiveEntities.clear();
            _activeEntities.clear();
        }
    };

    class BaseSystem {
        friend World;

        public:
        BaseSystem(const netero::set<netero::type_id> &includeFilterSet,
                   const netero::set<netero::type_id> &excludeFilterSet)
            : _includeFilterSet(includeFilterSet), _excludeFilterSet(excludeFilterSet)
        {
        }
        virtual ~BaseSystem() = default;

        private:
        void generateCache(netero::set<EntityContainer *> &entities)
        {
            _cache.flush();
            for (auto *entity : entities) {
                auto &entityFiltersSet = entity->getComponentsFilter();
                if (_includeFilterSet.isSubsetOf(entityFiltersSet)) {
                    if (_excludeFilterSet.interWith(entityFiltersSet)) {
                        if (entity->status) {
                            _cache._activeEntities.emplace_back(entity);
                        }
                        else {
                            _cache._unactiveEntities.emplace_back(entity);
                        }
                    }
                }
            }
        }

        public:
        virtual void                        exec() = 0;
        const netero::set<netero::type_id> &_includeFilterSet;
        const netero::set<netero::type_id> &_excludeFilterSet;

        protected:
        SystemCache _cache;
    };

    template<
        typename IncludeComponent = ComponentFilter<>,
        typename ExcludeComponent = ComponentFilter<>,
        typename = std::enable_if<std::is_base_of<IncludeComponent, BaseComponentFilter>::value>,
        typename = std::enable_if<std::is_base_of<ExcludeComponent, BaseComponentFilter>::value>>
    class System: public BaseSystem {
        public:
        System(): BaseSystem(IncludeComponent::getFilter(), ExcludeComponent::getFilter()) {}
        ~System() override = default;

        const std::vector<Entity> &getActiveEntities() { return _cache._activeEntities; }

        const std::vector<Entity> &getUnActiveEntities() { return _cache._unactiveEntities; }
    };
} // namespace ecs
} // namespace netero
