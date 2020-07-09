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

#include <netero/patterns/IObserver.hpp>
#include <netero/slot.hpp>

namespace netero {

template<class T>
class signal;

/**
	 * @class signal
	 * @see netero::slot
	 * @brief Event container that could emit signals.
	 * @tparam rType Return type of the signals.
	 * @tparam argsType Variadic types of arguments of the signal.
	 * The signal container propagate a function call to multiple slot.
	 */
template<class rType, class... argsType>
class signal<rType(argsType...)>: public IConnectibleDelegate {
    public:
    using Signature = rType(argsType...);

    signal() = default;

    /**
	     * @brief Disconnect automatically all connected slots.
	     */
    ~signal() override { reset(); };

    /**
		 * @brief Emit the signal.
		 * @param args forwarded to slots.
		 * This will emit a call to any connected slots.
		 */
    void operator()(argsType... args)
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        for (netero::slot<rType(argsType...)> *slot : _slots) {
            (*slot)(args...);
        }
    }

    /**
		 * @brief Connect a slot.
		 * @param connectable slot.
		 */
    void connect(IConnectibleDelegate *connectable) final
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        _slots.push_back(reinterpret_cast<netero::slot<Signature> *>(connectable));
        connectable->connect(this);
    }

    /**
		 * @brief Disconnect a slot.
		 * @param connectable slot already connected.
		 */
    void disconnect(IConnectibleDelegate *connectable) final
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        auto slot = reinterpret_cast<netero::slot<Signature> *>(connectable);
        auto it = std::find(_slots.begin(), _slots.end(), slot);
        if (it != _slots.end()) {
            _slots.erase(it);
        }
    }

    /**
		 * @return int - number of slot connected.
		 */
    int size() { return _slots.size(); }

    /**
		 * @brief Reset the signal by disconnect it from all its slots.
		 */
    void reset()
    {
        std::scoped_lock<std::mutex> lock(_slotsMutex);
        for (netero::slot<Signature> *slot : _slots) {
            slot->disconnect(this);
        }
        _slots.clear();
    }

    private:
    std::mutex                              _slotsMutex; /**< Slot access mutex. */
    std::vector<slot<rType(argsType...)> *> _slots;      /**< Slots vector. For connected slots. */
};
} // namespace netero
