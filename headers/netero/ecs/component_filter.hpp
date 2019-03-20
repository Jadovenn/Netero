/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <type_traits>
#include <cstddef>
#include <list>
#include "component.hpp"
#include "component_filter.hpp"

namespace netero {

    namespace ecs {

        struct BaseComponentFilter {
            virtual ~BaseComponentFilter() = 0;
        };

        template <typename ...Args>
        class ComponentFilter: public BaseComponentFilter
        {
        public:

            static const std::list<netero::type_id> &getTypes() {
                static std::list<netero::type_id> filter = decomposer<Args...>();
                return filter;
            }

        private:
            template<typename Last>
            static std::list<netero::type_id>	decomposer() {
                std::list<netero::type_id>	filter;
                static_assert(std::is_base_of<Component, Last>(), "Elem is not based on Component");
                filter.push_back(CompoentTypeID::getTypeID<Last>());
                return filter;
            }

            template<typename First, typename Second, typename ...Rest>
            static std::list<netero::type_id>	decomposer() {
                static_assert(std::is_base_of<Component, First>(), "Elem is not based on Component");
                auto list = decomposer<Second, Rest...>();
                list.push_back(CompoentTypeID::getTypeID<First>());
                return list;
            }
        };
    }
}
