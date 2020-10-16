/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <Netero/ECS/Entity.hpp>
#include <Netero/ECS/World.hpp>

namespace Netero::ECS {

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

bool Entity::Valid() const noexcept
{
    return _base != nullptr;
}

void Entity::Enable()
{
    if (_base) {
        _base->GetWorld()->EnableEntity(*this);
    }
}

void Entity::Disable()
{
    if (_base) {
        _base->GetWorld()->DisableEntity(*this);
    }
}

void Entity::Unregister()
{
    _base = nullptr;
}

void Entity::Kill()
{
    auto *entityWorld = this->_base->GetWorld();
    entityWorld->KillEntity(*this);
}
} // namespace Netero::ECS
