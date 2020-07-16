/*
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

namespace netero {

/**
 * Factory interface.
 */
template<typename T>
class IFactory {
	virtual ~IFactory() = 0
public:
	template<class... Args>
	T* Create(Args...);	

};
} // namespace netero

