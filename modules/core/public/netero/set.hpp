/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file set.hpp
 * @brief Set container header file.
 */

#include <set>

namespace netero {

/**
 * @brief Std set with subset comparison
 * @tparam __Type type holder
 */
template<typename T>
class set: public std::set<T> {
    public:
    set() = default;
    typedef typename std::set<T>::iterator iterator;

    // construct from a initializer list
    set(std::initializer_list<T> __il) { this->insert(__il.begin(), __il.end()); }

    // copy constructor from std::set
    explicit set(const std::set<T> &__copy) { this->insert(__copy.begin(), __copy.end()); }

    /**
     * @brief check if the actual set is a subset of the given set
     * @note the given set does not need to be a netero::set
     * @param other - set to compare
     * @return true if this is an subset, false otherwise
     */
    [[nodiscard]] bool isSubsetOf(const std::set<T> &other) const
    {
        iterator it_this = this->begin();
        iterator it_this_end = this->end();
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
     * @note the given set does not need to be a netero::set, in contrast
     * to isSubsetOf, interWith will stop at the first common element found
     * @param other - set to compare
     * @return true if their is a set which is a subset of both sets, false otherwise
     */
    [[nodiscard]] bool interWith(const std::set<T> &other) const
    {
        iterator it_this = this->begin();
        iterator it_this_end = this->end();
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
} // namespace netero
