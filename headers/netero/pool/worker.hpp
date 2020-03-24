/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <atomic>
#include <thread>

namespace netero::pool {
	class Worker {
	public:
		enum State {
			SLEEPING,
			WORKING,
			JOINING
		};
	public:
		Worker();
		~Worker();

		void handler();

	private:
		std::atomic<Worker::State>	_state;
		std::thread					_thread;
	};
}
