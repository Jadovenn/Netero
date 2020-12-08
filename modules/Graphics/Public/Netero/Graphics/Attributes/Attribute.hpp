/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/TypeId.hpp>

namespace Netero::Gfx {

class Attribute {
    public:
    virtual ~Attribute() = default;
};

using AttributesTypeID = Netero::TypeID<Attribute>;

} // namespace Netero::Gfx
