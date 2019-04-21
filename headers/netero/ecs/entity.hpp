/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <cstddef>
#include <string>
#include <map>
#include <list>
#include <exception>
#include <algorithm>
#include "component.hpp"
#include "netero/set.hpp"

namespace netero {
	namespace ecs {

		class World;

		class EntityContainer {
			struct id {
				std::string	name;
			};
		public:
			friend World;
			~EntityContainer();

			World	*getWorld();
			template<typename T, typename ...Args>
			T	&addComponent(Args&&... args);
			template<typename T>
			T	&getComponent();
			template<typename T>
			void	deleteComponent();

			const netero::set<netero::type_id>	&getComponentsFilter() const {
				return _componentsFilterSet;
			}
			bool	status;
		private:
			explicit EntityContainer(World *world, const std::string &name = "unnamed");
			World		*_world;
			id			id;
			std::map<netero::type_id, Component *>	_components;
			netero::set<netero::type_id>			_componentsFilterSet;
		};

		template<typename T, typename ...Args>
		T	&EntityContainer::addComponent(Args&&... args) {
			static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
			netero::type_id	componentID = ComponentTypeID::getTypeID<T>();
			auto it = _componentsFilterSet.find(componentID);
			if (it != _componentsFilterSet.end())
				throw std::runtime_error("One entitie could not own the same component twice.");
			T	*dataPtr = new (std::nothrow) T{ std::forward<Args>(args)... };
			if (!dataPtr)
				throw std::bad_alloc();
			_components[componentID] = dataPtr;
			_componentsFilterSet.insert(componentID);
			return *dataPtr;
		}

		template<typename T>
		T	&EntityContainer::getComponent() {
			static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
			auto it = _componentsFilterSet.find(ComponentTypeID::getTypeID<T>());
			if (it == _componentsFilterSet.end())
				throw std::runtime_error("Entity does not own T component.");
			return dynamic_cast<T&>(*_components.at(ComponentTypeID::getTypeID<T>()));
		}

		template<typename T>
		void	EntityContainer::deleteComponent() {
			static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
			netero::type_id	componentID = ComponentTypeID::getTypeID<T>();
			auto it = _componentsFilterSet.find(componentID);
			if (it == _componentsFilterSet.end())
				throw std::runtime_error("Entity does not own T component.");
			auto compIt = _components.find(componentID);
			delete (*compIt).second;
			_components.erase(compIt);
			_componentsFilterSet.erase(componentID);
		}

		class Entity
		{
		public:
			Entity();
			Entity(const Entity &rhs);
			Entity(EntityContainer *entityContainer);
			Entity	&operator=(EntityContainer *rhs);
			Entity	&operator=(const Entity &rhs);
			Entity	&operator=(Entity &&rhs);
			bool	operator==(const Entity &rhs);
			bool	operator==(const EntityContainer *rhs);
			EntityContainer	*operator->();
			virtual ~Entity() = default;

			bool	valid() const noexcept;
			void	enable();
			void	disable();
			void	kill();
			void	unregister();

		private:
			EntityContainer		*_base;
		};
	}
}
