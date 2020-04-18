/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file slots.hpp
 * @brief Slot specific exception.
 */

#include <exception>

namespace netero {

	/**
	 * Trowed while a slot does not have a functor.
	 */
	class bad_slot_call : std::exception {
	public:
		~bad_slot_call() override = default;

		[[nodiscard]] const char *what() const noexcept override {
			return "bad slot call";
		}
	};

}
