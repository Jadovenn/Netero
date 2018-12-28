/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <string>

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

			bool	status;
		private:
			EntityContainer(World *world, const std::string &name = "unnamed");
			World		*_world;
			id			id;
		};

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
			EntityContainer	*operator->();
			virtual ~Entity() = default;

			bool	valid() const noexcept;
			void	enable();
			void	disable();

		private:
			EntityContainer		*_base;
		};
	}
}
