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
    public:
    virtual ~IFactory() = 0;
    template<class... Args>
    T* Create(Args...);
};
} // namespace netero
