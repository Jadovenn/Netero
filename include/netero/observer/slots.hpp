/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <algorithm>
#include <functional>
#include <mutex>
#include <exception>
#include <vector>

#include <netero/observer/IObserverDelegate.hpp>
#include <netero/exception.hpp>

/**
 * @brief Slots
 * @category not [moveable]
 */

namespace netero {

	template <typename T>
	class slot;

	/**
	 * @brief base slot for function
	 * @tparam _rType
	 * @tparam _ArgsType
	 */
	template <typename _rType, typename ..._ArgsType>
	class slot<_rType(_ArgsType...)>: public IObserverDelegate {
	public:

		explicit slot(_rType(*func_ptr)(_ArgsType...))
			:	_function(func_ptr)
		{}

		explicit slot(std::function<_rType(_ArgsType...)> f)
			:	_function(f.target())
		{}

		template <typename _Base>
		slot(_rType(_Base::*methode)(_ArgsType...), _Base *base) {
			_function = [methode, base] (_ArgsType ...args) -> _rType {
				return (base->*methode)(std::forward<_ArgsType>(args)...);
			};
		}

		slot(slot<_rType(_ArgsType...)> &copy)
			:	_function(copy._func_ptr)
		{}

		~slot() override {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			for (auto *signal: _signals) {
				signal->disconnect(this);
			}
		}

		virtual _rType    operator()(_ArgsType... args) {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			if (_function) {
				return _function(std::forward<_ArgsType>(args)...);
			}
			throw netero::bad_slot_call();
		}

		void disconnect(IObserverDelegate *delegate) override {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			auto it = std::find_if(_signals.begin(),
					_signals.end(),
					[delegate] (auto *item) {
				return delegate == item;
			});
			if (it != _signals.end()) {
				_signals.erase(it);
			}
		}

		void connect(IObserverDelegate *delegate) override {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			_signals.push_back(delegate);
		}

	private:
		std::mutex								_sigMutex;
		std::function<_rType(_ArgsType...)>		_function;
		std::vector<netero::IObserverDelegate*>	_signals;
	};

}

