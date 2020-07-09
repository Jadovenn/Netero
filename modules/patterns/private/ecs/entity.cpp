/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "netero/ecs/entity.hpp"

#include "netero/ecs/world.hpp"

namespace netero::ecs {

Entity::Entity(): _base(nullptr)
{
}

Entity::Entity(const Entity &rhs)
{
    _base = rhs._base;
}

Entity::Entity(EntityContainer *entityContainer)
{
    _base = entityContainer;
}

Entity &Entity::operator=(EntityContainer *rhs)
{
    _base = rhs;
    return *this;
}

Entity &Entity::operator=(const Entity &rhs)
{
    _base = rhs._base;
    return *this;
}

Entity &Entity::operator=(Entity &&rhs) noexcept
{
    _base = rhs._base;
    rhs._base = nullptr;
    return *this;
}

EntityContainer *Entity::operator->()
{
    return _base;
}

bool Entity::operator==(const Entity &rhs)
{
    return _base == rhs._base;
}

bool Entity::operator==(const EntityContainer *rhs)
{
    return _base == rhs;
}

bool Entity::valid() const noexcept
{
    return _base != nullptr;
}

void Entity::enable()
{
    if (_base) {
        _base->getWorld()->enableEntity(*this);
    }
}

void Entity::disable()
{
    if (_base) {
        _base->getWorld()->disableEntity(*this);
    }
}

void Entity::unregister()
{
    _base = nullptr;
}

void Entity::kill()
{
    auto *entityWorld = this->_base->getWorld();
    entityWorld->killEntity(*this);
}
} // namespace netero::ecs
