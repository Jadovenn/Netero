/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <type_traits>

namespace netero {
	namespace ecs {

		template<typename IncludeComponent, typename ExcludeComponent>
		class System {
		public:
			virtual ~System() = default;
		};
	}
}
