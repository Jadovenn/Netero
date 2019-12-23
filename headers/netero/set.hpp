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
	template <typename __Type>
	class set : public std::set<__Type> {
	public:
		set() = default;
		typedef typename std::set<__Type>::iterator iterator;

		// construct from a initializer list
		set(std::initializer_list<__Type> __il) {
			this->insert(__il.begin(), __il.end());
		}

		// copy constructor from std::set
		explicit set(const std::set<__Type> &__copy) {
			this->insert(__copy.begin(), __copy.end());
		}

		/**
		 * @brief check if the actual set is a subset of the given set
		 * @note the given set does not need to be a netero::set
		 * @param other - set to compare
		 * @return true if this is an subset, false otherwise
		 */
		bool	isSubsetOf(const std::set<__Type> &other) const {
			iterator		it_this = this->begin();
			iterator		it_this_end = this->end();
			if (other.size() == 0)
				return false;
			while (it_this != it_this_end) {
				auto it = other.find(*it_this);
				if (it == other.end()) {
					return false;
				}
				++it_this;
			}
			return true;
		} // O(n, m) = n log(m), where n is size of this and m size of other

		/**
		 * @brief check if two sets has an intersection
		 * @note the given set does not need to be a netero::set, in contraste
		 * to isSubsetOf, interWith will stop at the first common element found
		 * @param other - set to compare
		 * @return true if their is a set which is a subset of both sets, false otherwise
		 */
		bool	interWith(const std::set<__Type>& other) {
			iterator		it_this = this->begin();
			iterator		it_this_end = this->end();
			if (other.size() == 0) {
				return false;
			}
			while (it_this != it_this_end) {
				auto it = other.find(*it_this);
				if (it != other.end()) {
					return true;
				}
				++it_this;
			}
			return false;
		} // O(n, m) = n log(m), where n is size of this and m size of other
	};
}
