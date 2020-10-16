/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <exception>
#include <list>
#include <map>
#include <string>

#include <Netero/ECS/Component.hpp>
#include <Netero/Set.hpp>

namespace Netero::ECS {

class World;

class EntityContainer {
    struct id {
        std::string name;
    };

    public:
    friend World;
    ~EntityContainer();

    World *GetWorld();
    template<typename T, typename... Args>
    T &AddComponent(Args &&... args);
    template<typename T>
    T &GetComponent();
    template<typename T>
    void DeleteComponent();

    [[nodiscard]] const Netero::Set<Netero::type_id> &GetComponentsFilter() const
    {
        return _componentsFilterSet;
    }
    bool status;

    private:
    explicit EntityContainer(World *world, const std::string &name = "unnamed");
    World *                                _world;
    id                                     id;
    std::map<Netero::type_id, Component *> _components;
    Netero::Set<Netero::type_id>           _componentsFilterSet;
};

template<typename T, typename... Args>
T &EntityContainer::AddComponent(Args &&... args)
{
    static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
    Netero::type_id componentID = ComponentTypeID::GetTypeID<T>();
    auto            it = _componentsFilterSet.find(componentID);
    if (it != _componentsFilterSet.end())
        throw std::runtime_error("One entity could not own the same component twice.");
    T *dataPtr = new (std::nothrow) T { std::forward<Args>(args)... };
    if (!dataPtr)
        throw std::bad_alloc();
    _components[componentID] = dataPtr;
    _componentsFilterSet.insert(componentID);
    return *dataPtr;
}

template<typename T>
T &EntityContainer::GetComponent()
{
    static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
    auto it = _componentsFilterSet.find(ComponentTypeID::GetTypeID<T>());
    if (it == _componentsFilterSet.end())
        throw std::runtime_error("Entity does not own T component.");
    return dynamic_cast<T &>(*_components.at(ComponentTypeID::GetTypeID<T>()));
}

template<typename T>
void EntityContainer::DeleteComponent()
{
    static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
    Netero::type_id componentID = ComponentTypeID::GetTypeID<T>();
    auto            it = _componentsFilterSet.find(componentID);
    if (it == _componentsFilterSet.end())
        throw std::runtime_error("Entity does not own T component.");
    auto compIt = _components.find(componentID);
    delete (*compIt).second;
    _components.erase(compIt);
    _componentsFilterSet.erase(componentID);
}

class Entity {
    public:
    Entity();
    Entity(const Entity &rhs);
    explicit Entity(EntityContainer *entityContainer);
    Entity &         operator=(EntityContainer *rhs);
    Entity &         operator=(const Entity &rhs);
    Entity &         operator=(Entity &&rhs) noexcept;
    bool             operator==(const Entity &rhs);
    bool             operator==(const EntityContainer *rhs);
    EntityContainer *operator->();
    virtual ~Entity() = default;

    bool Valid() const noexcept;
    void Enable();
    void Disable();
    void Kill();
    void Unregister();

    private:
    EntityContainer *_base;
};

} // namespace Netero::ECS
