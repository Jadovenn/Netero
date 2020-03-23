/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>
#include <netero/audio/device.hpp>

#include <chrono>
#include <thread>

int main() {
    netero::audio::signals::sinusoid    a_minor(0.4, 440, 0);
    //netero::audio::signals::sinusoid    e_minor(0.4, 650, 0);
    a_minor.play();
    //e_minor.play();
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    a_minor.stop();
    //e_minor.stop();
    return 0;
}
