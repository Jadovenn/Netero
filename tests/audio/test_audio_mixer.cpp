/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>
#include <netero/audio/device.hpp>

#include <chrono>
#include <thread>

int main() {
    netero::audio::signals::sinusoid    _mi(0.01, 329.63, 0);
    //netero::audio::signals::sinusoid    _sol(0.01, 440, 0);
    _mi.play();
    //_sol.play();
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    _mi.stop();
    //_sol.stop();
    return 0;
}
