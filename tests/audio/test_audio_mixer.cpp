/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>
#include <netero/audio/device.hpp>

#include <chrono>
#include <thread>

int main() {
    netero::audio::signals::sinusoid    _la(0.01, 440, 0);
    netero::audio::signals::sinusoid    _do(0.01, 261.63, 0);
    netero::audio::signals::sinusoid    _mi(0.01, 329.63, 0);
    _la.play();
    _do.play();
    _mi.play();
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    _la.stop();
    _do.stop();
    _mi.play();
    return 0;
}
