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

#include <netero/observer/IObserverDelegate.hpp>
#include <netero/observer/slot.hpp>

namespace netero {

	template<class T>
	class signal;

	/**
	 * @class signal
	 * @see netero::slot
	 * @brief Event container that could emit signals.
	 * @tparam _rType Return type of the signals.
	 * @tparam _ArgsType Variadic types of arguments of the signal.
	 * The signal container propagate a function call to multiple slot
	 */
	template<class _rType, class ..._ArgsType>
	class signal<_rType(_ArgsType...)>:  public IObserverDelegate {
	public:
		using Signature = _rType(_ArgsType...);

		~signal() override {
			flush();
		};

		void    operator()(_ArgsType... args) {
			std::scoped_lock<std::mutex>	lock(_slotsMutex);
			for (netero::slot<_rType(_ArgsType...)> *slot: _slots) {
				(*slot)(args...);
			}
		}

		void 	connect(IObserverDelegate *connectable) final {
			std::scoped_lock<std::mutex>	lock(_slotsMutex);
			_slots.push_back(reinterpret_cast<netero::slot<Signature>*>(connectable));
			connectable->connect(this);
		}

		void 	disconnect(IObserverDelegate *connectable) final {
			std::scoped_lock<std::mutex>	lock(_slotsMutex);
			auto slot = reinterpret_cast<netero::slot<Signature>*>(connectable);
			auto it = std::find_if(_slots.begin(), _slots.end(), [slot] (auto* item) {
				return slot == item;
			});
			if (it != _slots.end()) {
				_slots.erase(it);
			}
		}

		int		size() {
			return _slots.size();
		}

		void	flush() {
			std::scoped_lock<std::mutex>	lock(_slotsMutex);
			for (netero::slot<Signature >* slot : _slots) {
				slot->disconnect(this);
			}
		}

	private:
		std::mutex									_slotsMutex;
		std::vector<slot<_rType(_ArgsType...)>*>	_slots;
	};
}

