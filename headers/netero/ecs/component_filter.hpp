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
#include "netero/set.hpp"

namespace netero {

    namespace ecs {

        struct BaseComponentFilter {
            virtual ~BaseComponentFilter() = 0;
        };

        template <typename ...Args>
        class ComponentFilter: public BaseComponentFilter
        {
        public:

            static const netero::set<netero::type_id> &getFilter() {
				static netero::set<netero::type_id>	filter;
				static bool 						isDecomposed = false;
				if (!isDecomposed) {
					decomposer<Args...>(filter);
					isDecomposed = true;
				}
                return filter;
            }

        private:
            template<typename Last>
            static void		decomposer(netero::set<netero::type_id> &filter) {
                static_assert(std::is_base_of<Component, Last>(), "Elem is not based on Component");
                filter.insert(CompoentTypeID::getTypeID<Last>());
            }

            template<typename First, typename Second, typename ...Rest>
            static void 	decomposer(netero::set<netero::type_id> &filter) {
                static_assert(std::is_base_of<Component, First>(), "Elem is not based on Component");
                decomposer<Second, Rest...>(filter);
                filter.insert(CompoentTypeID::getTypeID<First>());
            }
        };
    }
}
