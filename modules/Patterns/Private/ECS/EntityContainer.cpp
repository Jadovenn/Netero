/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <Netero/ECS/Entity.hpp>

namespace Netero::ECS {

EntityContainer::EntityContainer(World *world, const std::string &name)
    : id { name }, _world(world), status(false)
{
}

EntityContainer::~EntityContainer()
{
    for (auto &comp : _components) {
        delete comp.second;
    }
}

World *EntityContainer::GetWorld()
{
    return _world;
}

} // namespace Netero::ECS
