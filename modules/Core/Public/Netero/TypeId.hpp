/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file TypeId.hpp
 * @brief A compile time typeid mechanism.
 */

#include <atomic>
#include <cstddef>

namespace Netero {

using type_id = std::size_t;

/**
 * @brief A compile time TypeID mechanism.
 */
template<typename BaseClass>
class TypeID {
    public:
    /**
     * @brief Assign a unique id per class type.
     * @return The unique ID assigned to the template parameter.
     */
    template<typename SubType>
    static type_id GetTypeID()
    {
        static const type_id localTypeID = myBaseTypeCounter++;
        return localTypeID;
    }

    private:
    static std::atomic<type_id> myBaseTypeCounter; /**< Id counter. */
};

template<typename BaseClass>
std::atomic<type_id> TypeID<BaseClass>::myBaseTypeCounter { 0 };

} // namespace Netero
