/*
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>

namespace netero::patterns {

/**
 * Factory interface.
 */
template<typename T>
class IFactory {
    public:
    virtual ~IFactory() = 0;

    template<class ...Args>
    static std::unique_ptr<T> Create(Args... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

};
} // namespace netero
