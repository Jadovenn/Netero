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

			bool	status;
		private:
			EntityContainer(World *world, const std::string &name = "unnamed");
			World		*_world;
			id			id;
			std::map<netero::type_id, Component *>	_components;
			std::list<netero::type_id>				_componentsIdSet;
		};

		template<typename T, typename ...Args>
		T	&EntityContainer::addComponent(Args&&... args) {
			static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
			netero::type_id	componentID = CompoentTypeID::getTypeID<T>();
			bool isExisting = std::binary_search(_componentsIdSet.begin(), _componentsIdSet.end(), componentID);
			if (isExisting)
				throw std::runtime_error("One entitie could not own the same component twice.");
			T	*dataPtr = new (std::nothrow) T{ std::forward<Args>(args)... };
			if (!dataPtr)
				throw std::bad_alloc();
			_components[componentID] = dataPtr;
			_componentsIdSet.push_back(componentID);
			_componentsIdSet.sort();
			return *dataPtr;
		}

		template<typename T>
		T	&EntityContainer::getComponent() {
			static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
			bool isExisting = std::binary_search(_componentsIdSet.begin(), _componentsIdSet.end(), CompoentTypeID::getTypeID<T>());
			if (!isExisting)
				throw std::runtime_error("Entity does not own T component.");
			return dynamic_cast<T&>(*_components.at(CompoentTypeID::getTypeID<T>()));
		}

		template<typename T>
		void	EntityContainer::deleteComponent() {
			static_assert(std::is_base_of<Component, T>(), "T is not based on Component.");
			netero::type_id	componentID = CompoentTypeID::getTypeID<T>();
			bool isExisting = std::binary_search(_componentsIdSet.begin(), _componentsIdSet.end(), componentID);
			if (!isExisting)
				throw std::runtime_error("Entity does not own T component.");
			auto compIt = _components.find(componentID);
			delete (*compIt).second;
			_components.erase(compIt);
			_componentsIdSet.remove(componentID);
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
