/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <functional>
#include "netero/pool/worker.hpp"

netero::pool::Worker::Worker()
	:	_state(Worker::State::SLEEPING),
		_thread(std::bind(&Worker::handler, this))
{}

netero::pool::Worker::~Worker() {
	_state.store(Worker::State::JOINING, std::memory_order::memory_order_release);
	_thread.join();
}

void	netero::pool::Worker::handler() {
	do {
		std::this_thread::yield();
	} while(_state.load(std::memory_order::memory_order_acquire) != Worker::State::JOINING);
}
