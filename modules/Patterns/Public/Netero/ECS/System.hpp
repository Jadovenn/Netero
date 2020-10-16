/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <map>
#include <type_traits>
#include <vector>

#include <Netero/ECS/ComponentFilter.hpp>
#include <Netero/ECS/Entity.hpp>
#include <Netero/Set.hpp>

namespace Netero::ECS {

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
    BaseSystem(const Netero::Set<Netero::type_id> &includeFilterSet,
               const Netero::Set<Netero::type_id> &excludeFilterSet)
        : _includeFilterSet(includeFilterSet), _excludeFilterSet(excludeFilterSet)
    {
    }
    virtual ~BaseSystem() = default;

    private:
    void generateCache(Netero::Set<EntityContainer *> &entities)
    {
        _cache.flush();
        for (auto *entity : entities) {
            auto &entityFiltersSet = entity->GetComponentsFilter();
            if (_includeFilterSet.IsSubsetOf(entityFiltersSet)) {
                if (_excludeFilterSet.InterWith(entityFiltersSet)) {
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
    const Netero::Set<Netero::type_id> &_includeFilterSet;
    const Netero::Set<Netero::type_id> &_excludeFilterSet;

    protected:
    SystemCache _cache;
};

template<typename IncludeComponent = ComponentFilter<>,
         typename ExcludeComponent = ComponentFilter<>,
         typename = std::enable_if<std::is_base_of<IncludeComponent, BaseComponentFilter>::value>,
         typename = std::enable_if<std::is_base_of<ExcludeComponent, BaseComponentFilter>::value>>
class System: public BaseSystem {
    public:
    System(): BaseSystem(IncludeComponent::GetFilter(), ExcludeComponent::GetFilter()) {}
    ~System() override = default;

    const std::vector<Entity> &GetActiveEntities() { return _cache._activeEntities; }

    const std::vector<Entity> &GetUnActiveEntities() { return _cache._unactiveEntities; }
};

} // namespace Netero::ECS
