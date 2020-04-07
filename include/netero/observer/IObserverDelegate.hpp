/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

/**
 * @file IObserverDelegate.hpp
 * @brief Observer delegate interface.
 */

namespace netero {
	/**
	 * @interface IObserverDelegate
	 * @brief Connectable delegate object
	 */
	class IObserverDelegate {
	public:
		virtual ~IObserverDelegate() = default;

		/**
		 * @brief disconnect given object to actual obs/subject
		 */
		virtual void disconnect(IObserverDelegate*) = 0;

		/**
		 * @brief connect to other obs/subject
		 */
		 virtual void connect(IObserverDelegate*) = 0;
	};
	
}
