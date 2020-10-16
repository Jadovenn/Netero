/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file signal.hpp
 * @brief Signal emitter container.
 */

#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <type_traits>

#include <Netero/Patterns/IObserver.hpp>
#include <Netero/Slot.hpp>

namespace Netero {

template<class T>
class Signal;

/**
 * @class signal
 * @see netero::slot
 * @brief Event container that could emit signals.
 * @tparam rType Return type of the signals.
 * @tparam argsType Variadic types of arguments of the signal.
 * The signal container propagate a function call to multiple slot.
 */
template<class rType, class... argsType>
class Signal<rType(argsType...)>: public IConnectibleDelegate {
    public:
    using Signature = rType(argsType...);

    Signal() = default;

    /**
     * @brief Disconnect automatically all connected slots.
     */
    ~Signal() override { Reset(); };

    /**
     * @brief Emit the signal.
     * @param args forwarded to slots.
     * This will emit a call to any connected slots.
     */
    void operator()(argsType... args)
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        for (Netero::Slot<rType(argsType...)> *slot : _slots) {
            (*slot)(args...);
        }
    }

    /**
     * @brief Connect a slot.
     * @param connectable slot.
     */
    void Connect(IConnectibleDelegate *connectable) final
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        _slots.push_back(reinterpret_cast<Netero::Slot<Signature> *>(connectable));
        connectable->Connect(this);
    }

    /**
     * @brief Disconnect a slot.
     * @param connectable slot already connected.
     */
    void Disconnect(IConnectibleDelegate *connectable) final
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        auto slot = reinterpret_cast<Netero::Slot<Signature> *>(connectable);
        auto it = std::find(_slots.begin(), _slots.end(), slot);
        if (it != _slots.end()) {
            _slots.erase(it);
        }
    }

    /**
     * @return int - number of slot connected.
     */
    int Size() { return _slots.size(); }

    /**
     * @brief Reset the signal by disconnect it from all its slots.
     */
    void Reset()
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        for (Netero::Slot<Signature> *slot : _slots) {
            slot->Disconnect(this);
        }
        _slots.clear();
    }

    private:
    std::mutex                              _slotsMutex; /**< Slot access mutex. */
    std::vector<Slot<rType(argsType...)> *> _slots;      /**< Slots vector. For connected slots. */
};
} // namespace Netero
