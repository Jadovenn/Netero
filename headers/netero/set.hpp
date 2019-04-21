/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <set>

namespace netero {

	/**
	 * @brief a set container that follow the math definition
	 * base of the std::set
	 * @tparam value_type - type holder
	 */
	template <typename value_type>
	class set : public std::set<value_type> {
	public:
		set() = default;

		// construct from a initializer list
		set(std::initializer_list<value_type> __il) {
			this->insert(__il.begin(), __il.end());
		}

		// copy constructor from std::ste
		explicit set(const std::set<value_type> &__copy) {
			this->insert(__copy.begin(), __copy.end());
		}

		/**
		 * @brief check if the actual set is a subset of the given set
		 * @note the given set does not need to be a netero::set
		 * @param other - set to compare
		 * @return true is this is an subset, false otherwise
		 */
		bool	isSubsetOf(const std::set<value_type> &other) {
			typename std::set<value_type>::iterator		it_this = this->begin();
			typename std::set<value_type>::iterator		it_this_end = this->end();
			typename std::set<value_type>::iterator		it_other = other.begin();
			typename std::set<value_type>::iterator		it_other_end = other.end();
			while (it_this != it_this_end) {
				while (*it_this != *it_other) {
					it_other++;
					if (it_other == it_other_end)
						return false;
				}
				it_this++;
			}
			return true;
		} // O(n) = n, where n is size of other
	};
}
