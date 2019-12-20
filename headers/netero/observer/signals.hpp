/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <map>
#include <algorithm>
#include <functional>
#include <type_traits>

#include "IObserverDelegate.hpp"
#include "slots.hpp"

namespace netero {

	template<class T>
	class signals;

	template<class _rType, class ..._ArgsType>
	class signals<_rType(_ArgsType...)>:  public IObserverDelegate {
	public:
		using Signature = _rType(_ArgsType...);

		~signals() override {
			for (netero::slot<Signature >*slot: _slots) {
				slot->disconnect(this);
			}
		};

		void    operator()(_ArgsType... args) {
			for (netero::slot<_rType(_ArgsType...)> *slot: _slots) {
				(*slot)(args...);
			}
		}

		void 	connect(IObserverDelegate *connectable) final {
			_slots.push_back(reinterpret_cast<netero::slot<Signature>*>(connectable));
			connectable->connect(this);
		}

		void 	disconnect(IObserverDelegate *connectable) final {
			auto slot = reinterpret_cast<netero::slot<Signature>*>(connectable);
			auto it = std::find_if(_slots.begin(), _slots.end(), [slot] (auto* item) {
				return slot == item;
			});
			if (it != _slots.end()) {
				_slots.erase(it);
			}
		}

	private:
		std::vector<slot<_rType(_ArgsType...)>*>	_slots;
	};
}
