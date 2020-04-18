/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file slot.hpp
 * @brief Slot container that execute a functor while a connected signal is emitted.
 */

#include <algorithm>
#include <functional>
#include <mutex>
#include <exception>
#include <vector>

#include <netero/observer/IObserverDelegate.hpp>
#include <netero/exception.hpp>

namespace netero {

	template <typename T>
	class slot;

	/**
	 * @brief Slot container to hold a functor object.
	 * @tparam _rType Return type
	 * @tparam _ArgsType Arguments type.
	 * A slot is a container used to hold a callable object such as a
	 * function, functor or a method. It manage connection to signals and will
	 * automatically disconnect from any signal while destructed.
	 */
	template <typename _rType, typename ..._ArgsType>
	class slot<_rType(_ArgsType...)>: public IObserverDelegate {
	public:

		/**
		 * @brief Instantiate a slot with empty functor.
		 */
		slot() = default;

		/**
		 * @brief Instantiate a slot with a function ptr.
		 * @param func_ptr Function pointer.
		 */
		explicit slot(_rType(*func_ptr)(_ArgsType...)) noexcept
			:	_function(func_ptr)
		{}

		/**
		 * @brief Instantiate a slot from a standard function container.
		 * @param f Standard function object.
		 */
		explicit slot(const std::function<_rType(_ArgsType...)> &f) noexcept
			:	_function(f)
		{}

		/**
		 * @brief Instantiate a slot from a class method ptr.
		 * @tparam _Base Type of the class.
		 * @param method Method pointer from class _Base.
		 * @param base Pointer to instance of the class.
		 */
		template <typename _Base>
		slot(_rType(_Base::*method)(_ArgsType...), _Base *base) {
			_function = [method, base] (_ArgsType ...args) -> _rType {
				return (base->*method)(std::forward<_ArgsType>(args)...);
			};
		}

		/**
		 * @brief Copy constructor.
		 * @param copy
		 * This will also copy signal connection.
		 */
		slot(const slot<_rType(_ArgsType...)> &copy)
			:	_function(copy._function)
		{
				for (auto signal: copy._signals) {
					signal->connect(this);
				}
		}

		/**
		 * @brief copy operator
		 * @param copy arg
		 * @return
		 */
		slot<_rType(_ArgsType...)>&  operator=(const slot<_rType(_ArgsType...)>& copy) {
			for (auto *signal : this->_signals) {
				signal->disconnect(this);
			}
			{
				std::scoped_lock<std::mutex>	lock(_sigMutex);
				this->_function = copy._function;
				this->_signals.clear();
			}
			for (auto *signal : copy._signals) {
				signal->connect(this);
			}
			return *this;
		}

		/**
		 * @brief move operator
		 * @param move arg
		 * @return 
		 */
		slot<_rType(_ArgsType...)>& operator=(slot<_rType(_ArgsType...)>&& copy) {
			std::vector<netero::IObserverDelegate*> tmp_signal_move;

			for (auto* signal : this->_signals) {
				signal->disconnect(this);
			}
			{
				std::scoped_lock<std::mutex>	lock(_sigMutex);
				this->_function = std::move(copy._function);
				this->_signals.clear();
				tmp_signal_move = std::move(copy._signals);
			}
			for (auto* signal : tmp_signal_move) {
				signal->connect(this);
			}
			return *this;
		}

		/**
		 * @brief Destructor, disconnect from any connected signal
		 */
		~slot() override {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			for (auto *signal: _signals) {
				signal->disconnect(this);
			}
		}

		/**
		 * @brief Set the functor to the given class method.
		 * @tparam _Base Type of the class.
		 * @param method Method pointer from class _Base.
		 * @param base Pointer to instance of the class.
		 */
		template <typename _Base>
		void	set(_rType(_Base::* method)(_ArgsType...), _Base* base) {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			_function = [method, base] (_ArgsType ...args) -> _rType {
				return (base->*method)(std::forward<_ArgsType>(args)...);
			};
		}

		/**
		 * @brief Set functor container.
		 * @param function_ptr Function pointer.
		 * Set the internal functor container to the given function pointer.
		 */
		void	set(_rType(*function_ptr)(_ArgsType...)) {
			_function = function_ptr;
		}

		/**
		 * @brief Set functor container.
		 * @brief Set functor container.
		 * @param functor Standard function container.
		 * Set the inter functor container to the give functor ref using
		 * the copy operator.
		 */
		void	set(const std::function<_rType(_ArgsType...)> &functor) {
			_function = functor;
		}

		/**
		 * @brief call
		 * @param args
		 * @return
		 */
		virtual _rType    operator()(_ArgsType... args) {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			if (_function) {
				return _function(std::forward<_ArgsType>(args)...);
			}
			throw netero::bad_slot_call();
		}


		/**
		 * @brief Test if the slot is callable.
		 * @return true if the internal functor is callable, false otherwise
		 */
		explicit operator bool() {
			return static_cast<bool>(_function);
		}


		/**
		 * @brief Disconnect the slot from the given signal.
		 * @param delegate Notifier that emit signals.
		 * This will disconnect the slot to the given notifier.
		 * Then the slot will stop receiving emitted signal.
		 */
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

		/**
		 * @brief Connect slot to a notifier.
		 * @param delegate Notifier that emit signals.
		 * This while connect the slot to the given signals. Then the
		 * slot could receive signal.
		 */
		void connect(IObserverDelegate *delegate) override {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			_signals.push_back(delegate);
		}

		/**
		 * @brief Get the number of notifier connected to the slot.
		 */
		int count_signal() {
			return this->_signals.size();
		}

	private:
		std::mutex								_sigMutex; /**< Mutex to prevent data race over signal vector. */
		std::function<_rType(_ArgsType...)>		_function; /**< Standard functor container. */
		std::vector<netero::IObserverDelegate*>	_signals; /**< Signal vector to keep track of lifecycle connection. */
	};
}
