/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>
#include <netero/audio/mixer.hpp>
#include <netero/audio/engine.hpp>

#include <chrono>
#include <thread>

int main() {
    netero::audio::engine   audio_engine;
    netero::audio::mixer    root_mixer(audio_engine);
    netero::audio::signals::sinusoid    _la(0.01, 440, 0);
    netero::audio::signals::sinusoid    _do(0.01, 261.63, 0);
    netero::audio::signals::sinusoid    _mi(0.01, 329.63, 0);
    root_mixer.connect(&_la);
    root_mixer.connect(&_do);
    root_mixer.connect(&_mi);
    root_mixer.play();
    audio_engine.startRender();
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    audio_engine.stopRender();
    return 0;
}
