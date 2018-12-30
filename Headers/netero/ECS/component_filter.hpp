/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <type_traits>
#include <cstddef>
#include <list>
#include "component.hpp"

namespace netero {

	namespace ecs {

		template <typename ...Args>
		class ComponentFilter
		{
		public:

			template<typename Last>
			static std::list<std::size_t>	decomposer() {
				std::list<std::size_t>	filter;
				static_assert(std::is_base_of<Component, Last>(), "Elem is not based on Component");
				filter.push_back(CompoentTypeID::getTypeID<Last>());
				return filter;
			}
			
			template<typename First, typename Second, typename ...Rest>
			static std::list<std::size_t>	decomposer() {
				static_assert(std::is_base_of<Component, First>(), "Elem is not based on Component");
				auto list = decomposer<Second, Rest...>();
				list.push_back(CompoentTypeID::getTypeID<First>());
				return list;
			}

			static const std::list<std::size_t>	&getTypes() {
				static std::list<std::size_t> filter = decomposer<Args...>();
				return filter;
			}
		};
	}
}
