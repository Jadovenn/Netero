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
template<typename T, typename ...ARGS>
class IFactory {
    public:
    virtual ~IFactory() = default;

    virtual std::shared_ptr<T> Create(ARGS...) = 0;
};
} // namespace netero::patterns
