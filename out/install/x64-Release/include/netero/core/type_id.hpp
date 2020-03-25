/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <cstddef>
#include <atomic>

/**
 * @brief A compile Time TypeID mechanisme
 */
namespace netero {

	using type_id = std::size_t;

	template <typename BaseClass>
	class TypeID
	{
	public:

		template<typename SubType>
		static type_id	getTypeID() {
			static const type_id localTypeID = _baseTypeCounter++;
			return localTypeID;
		}

	private:
		static std::atomic<type_id>	_baseTypeCounter;
	};

	template<typename BaseClass>
	std::atomic<type_id>	TypeID<BaseClass>::_baseTypeCounter{0};

}
