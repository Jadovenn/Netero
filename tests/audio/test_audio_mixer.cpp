/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>
#include <netero/audio/device.hpp>

#include <chrono>
#include <thread>

int main() {
    netero::audio::device& audio_device = netero::audio::device::GetAudioDevice();
    netero::audio::signals::sinusoid    a_minor(0.006, 440, 0);

    audio_device.connect(&a_minor);
    a_minor.play();
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 3) {
        std::this_thread::yield();
    }
    a_minor.stop();
    audio_device.disconnect(&a_minor);
    return 0;
}
