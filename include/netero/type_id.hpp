/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file type_id.hpp
 * A compile time typeid mechanism.
 */

#include <cstddef>
#include <atomic>

namespace netero {

	using type_id = std::size_t;

	/**
	 * @brief A compile time TypeID mechanism.
	 */
	template <typename BaseClass>
	class TypeID
	{
	public:

		/**
		 * @brief Assign a unique id per class type.
		 * @return The unique ID assigned to the template parameter.
		 */
		template<typename SubType>
		static type_id	getTypeID() {
			static const type_id localTypeID = _baseTypeCounter++;
			return localTypeID;
		}

	private:
		static std::atomic<type_id>	_baseTypeCounter; /**< Id counter. */
	};

	template<typename BaseClass>
	std::atomic<type_id>	TypeID<BaseClass>::_baseTypeCounter{0};

}
