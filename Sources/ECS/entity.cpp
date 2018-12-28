/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "netero/ECS/entity.hpp"

namespace netero::ecs {

	Entity::Entity()
		:	_base(nullptr)
	{}

	Entity::Entity(const Entity &rhs) {
		_base = rhs._base;
	}

	Entity::Entity(EntityContainer *entityContainer) {
		_base = entityContainer;
	}

	Entity	&Entity::operator=(EntityContainer *rhs) {
		_base = rhs;
		return *this;
	}

	Entity	&Entity::operator=(const Entity &rhs) {
		_base = rhs._base;
		return *this;
	}

	Entity	&Entity::operator=(Entity &&rhs) {
		_base = rhs._base;
		rhs._base = nullptr;
		return *this;
	}

	EntityContainer		*Entity::operator->() {
		return _base;
	}

	bool	Entity::operator==(const Entity &rhs) {
		return _base == rhs._base;
	}

	bool	Entity::operator==(const EntityContainer *rhs) {
		return _base == rhs;
	}

	bool	Entity::valid() const noexcept {
		return _base ? true : false;
	}

	void	Entity::enable() {
		_base->status = true;
	}

	void	Entity::disable() {
		_base->status = false;
	}
	
	void	Entity::unregister() {
		_base = nullptr;
	}
}

