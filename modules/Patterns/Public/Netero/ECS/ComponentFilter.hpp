/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <cstddef>
#include <list>
#include <type_traits>

#include <Netero/ECS/Component.hpp>
#include <Netero/ECS/ComponentFilter.hpp>
#include <Netero/Set.hpp>

namespace Netero::ECS {

struct BaseComponentFilter {
    virtual ~BaseComponentFilter() = 0;
};

template<typename... Args>
class ComponentFilter: public BaseComponentFilter {
    private:
    template<typename Last>
    static void Decomposer(Netero::Set<Netero::type_id> &filter)
    {
        static_assert(std::is_base_of<Component, Last>(), "Elem is not based on Component");
        filter.insert(ComponentTypeID::GetTypeID<Last>());
    }

    template<typename First, typename Second, typename... Rest>
    static void Decomposer(Set<Netero::type_id> &filter)
    {
        static_assert(std::is_base_of<Component, First>(), "Elem is not based on Component");
        Decomposer<Second, Rest...>(filter);
        filter.insert(ComponentTypeID::GetTypeID<First>());
    }

    public:
    static const Set<Netero::type_id> &GetFilter()
    {
        static Set<Netero::type_id> filter;
        static bool                 isDecomposed = false;
        if (!isDecomposed) {
            Decomposer<Args...>(filter);
            isDecomposed = true;
        }
        return filter;
    }
};

template<>
class ComponentFilter<>: public BaseComponentFilter {
    public:
    static const Netero::Set<Netero::type_id> &GetFilter()
    {
        static Netero::Set<Netero::type_id> filter;
        return filter;
    }
};

} // namespace Netero::ECS
