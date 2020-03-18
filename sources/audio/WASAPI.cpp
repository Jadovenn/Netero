/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <netero/audio/engine.hpp>

class netero::audio::engine::impl {
private:

public:

	impl() {

	}

	~impl() {

	}

	void					registerCB(std::function<void(float*)> cb) {
		(void)cb;
	}

	netero::audio::RtCode	start() {
		return OK;
	}

	netero::audio::RtCode	stop() {
		return OK;
	}
};

netero::audio::engine::engine()
	:	pImpl {std::make_unique<netero::audio::engine::impl>()}
{}

void					netero::audio::engine::registerCB(std::function<void(float*)> cb) {
	pImpl->registerCB(cb);
}

netero::audio::RtCode	netero::audio::engine::start() {
	return pImpl->start();
}

netero::audio::RtCode	netero::audio::engine::stop() {
	return pImpl->stop();
}
