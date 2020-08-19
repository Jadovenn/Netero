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
    virtual ~IFactory() = default;

    virtual std::shared_ptr<T> Create() = 0;
};
} // namespace netero::patterns
