/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include "netero/Core/type_id.hpp"

namespace netero {
	namespace ecs {

		class Component {
		public:
			virtual ~Component() = default;

		};

		using CompoentTypeID = netero::TypeID<Component>;
	}
}
