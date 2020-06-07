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

/**
 * @file slot.hpp
 * @brief Function slot.
 */

namespace netero {

	template <typename T>
	class slot;

	/**
	 * @brief Slot container to hold a connectable functor object.
	 * @tparam rType Return type
	 * @tparam ArgsType Arguments type.
	 * A slot is a container used to hold a callable object such as a
	 * function, functor or a method. It manage connection to signals and will
	 * automatically disconnect from any signal while destructed.
	 */
	template <typename rType, typename ...ArgsType>
	class slot<rType(ArgsType...)>: public IObserverDelegate {
	public:

		/**
		 * @brief Instantiate a slot with empty functor.
		 */
		slot() = default;

		/**
		 * @brief Instantiate a slot with a function ptr.
		 * @param func_ptr Function pointer.
		 */
		explicit slot(rType(*func_ptr)(ArgsType...)) noexcept
			:	_function(func_ptr)
		{}

		/**
		 * @brief Instantiate a slot from a standard function container.
		 * @param f Standard function object.
		 */
		explicit slot(const std::function<rType(ArgsType...)> &f) noexcept
			:	_function(f)
		{}

		/**
		 * @brief Instantiate a slot from a class method ptr.
		 * @tparam _Base Type of the class.
		 * @param method Method pointer from class _Base.
		 * @param base Pointer to instance of the class.
		 */
		template <typename Base>
		slot(rType(Base::*method)(ArgsType...), Base *base) {
			_function = [method, base] (ArgsType ...args) -> rType {
				return (base->*method)(std::forward<ArgsType>(args)...);
			};
		}

		/**
		 * @brief Copy constructor.
		 * @param copy other slot.
		 * This will also copy signal connections.
		 */
		slot(const slot<rType(ArgsType...)> &copy)
			:	_function(copy._function)
		{
				for (auto signal: copy._signals) {
					signal->connect(this);
				}
		}

		/**
		 * @brief Move constructor.
		 * @param other slot to move.
		 * This will also move signal connection between slots.
		 */
		slot(slot<rType(ArgsType...)>&& other) noexcept {
			this->_function = std::move(other._function);
			for (auto* signal: other._signals) {
				signal->disconnect(&other);
				signal->connect(this);
			}
			other._signals.clear();
		}

		/**
		 * @brief Copy operator.
		 * @param copy other slot.
		 * @return *this
		 */
		slot<rType(ArgsType...)>&  operator=(const slot<rType(ArgsType...)>& copy) {
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
		 * @brief Move operator.
		 * @param other slot to move.
		 * @return *this
		 */
		slot<rType(ArgsType...)>& operator=(slot<rType(ArgsType...)>&& other) {
			std::vector<netero::IObserverDelegate*> tmp_signal_move;
			for (auto* signal : this->_signals) {
				signal->disconnect(this);
			}
			{
				std::scoped_lock<std::mutex>	lock(_sigMutex);
				this->_function = std::move(other._function);
				this->_signals.clear();
				for (auto* signal: other._signals) {
					signal->disconnect(&other);
					tmp_signal_move.push_back(signal);
				}
				other._signals.clear();
			}
			for (auto* signal : tmp_signal_move) {
				signal->connect(this);
			}
			return *this;
		}

		/**
		 * @brief Destructor, disconnect from any connected signal.
		 */
		~slot() override {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			for (auto *signal: _signals) {
				signal->disconnect(this);
			}
		}

		/**
		 * @brief Set the functor to the given class method.
		 * @tparam Base Type of the class.
		 * @param method Method pointer from class _Base.
		 * @param base Pointer to instance of the class.
		 */
		template <typename Base>
		void	set(rType(Base::* method)(ArgsType...), Base* base) {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			_function = [method, base] (ArgsType ...args) -> rType {
				return (base->*method)(std::forward<ArgsType>(args)...);
			};
		}

		/**
		 * @brief Set functor container.
		 * @param function_ptr Function pointer.
		 * Set the internal functor container to the given function pointer.
		 */
		void	set(rType(*function_ptr)(ArgsType...)) {
			_function = function_ptr;
		}

		/**
		 * @brief Set functor container.
		 * @brief Set functor container.
		 * @param functor Standard function container.
		 * Set the inter functor container to the give functor ref using
		 * the copy operator.
		 */
		void	set(const std::function<rType(ArgsType...)> &functor) {
			_function = functor;
		}

		/**
		 * @brief Perform a call on the stored callable object.
		 * @param args to pass to the callable object.
		 * @return rType
		 */
		virtual rType	operator()(ArgsType... args) {
			std::scoped_lock<std::mutex>	lock(_sigMutex);
			if (_function) {
				return _function(std::forward<ArgsType>(args)...);
			}
			throw std::bad_function_call();
		}

		/**
		 * @brief Test if the slot is callable.
		 * @return true if the internal functor is callable, false otherwise.
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
			auto it = std::find(_signals.begin(), _signals.end(), delegate);
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
			std::scoped_lock<std::mutex>    lock(_sigMutex);
			_signals.push_back(delegate);
		}

		/**
		 * @brief Get the number of notifier connected to the slot.
		 */
		int count_signal() {
			return this->_signals.size();
		}

	private:
		std::mutex                              _sigMutex; /**< Mutex to prevent data race over signal vector. */
		std::function<rType(ArgsType...)>       _function; /**< Standard functor container. */
		std::vector<netero::IObserverDelegate*> _signals; /**< Signal vector to keep track of lifecycle connection. */
	};
}
