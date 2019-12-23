/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <exception>

namespace netero {

	class bad_slot_call : std::exception {
	public:
		~bad_slot_call() override = default;

		[[nodiscard]] const char *what() const noexcept override {
			return "bad slot call";
		}
	};

}
