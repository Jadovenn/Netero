/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <Netero/TypeId.hpp>

namespace Netero::ECS {

class Component {
    public:
    virtual ~Component() = default;
};

using ComponentTypeID = TypeID<Component>;

} // namespace Netero::ECS
