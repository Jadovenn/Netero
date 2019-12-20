/**
 *  Created by Jean-Adrien DOMAGE on 14/12/2019.
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
