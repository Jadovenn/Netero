/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/signals.hpp>
#include <netero/audio/mixer.hpp>
#include <netero/audio/engine.hpp>

#include <iostream>
#include <chrono>
#include <thread>

static
void    deviceErrorHandler(const std::string& message) {
    std::cout << message << std::endl;
}

static 
void    deviceDisconnectionHandler(const netero::audio::device &device) {
    std::cout << "device: " << device.name << " has been disconnected." << std::endl;
}

static
netero::slot<void(const netero::audio::device&)>    deviceDisconnectionSlot(&deviceDisconnectionHandler);

int main() {
    // Create an audio engine and retrieve the default render device
    auto& audioEngine = netero::audio::engine::getInstance();
    audioEngine.deviceDisconnectedSig.connect(&deviceDisconnectionSlot);
    const auto& defaultDevice = audioEngine.getDefaultRenderDevice();

    // Set device error handler, to be notify if some errors happened
    netero::audio::DeviceErrorSlot  deviceErrorSlot(deviceErrorHandler);
    auto& deviceEvent = audioEngine.getDeviceEvents(defaultDevice);
    deviceEvent.deviceErrorSig.connect(&deviceErrorSlot);
    
    // Create the root mixer, and connect the engine and device
    netero::audio::mixer    rootMixer(audioEngine, defaultDevice);

    // Create some signals to render
    netero::audio::signals::sinusoid    _la(rootMixer, 0.01, 440, 0);
    netero::audio::signals::sinusoid    _do(rootMixer, 0.01, 261.63, 0);
    netero::audio::signals::sinusoid    _mi(rootMixer, 0.01, 329.63, 0);

    // Make sure the mixer is playing and start the device
    rootMixer.play();
    audioEngine.deviceStartRendering(defaultDevice);

    // Wait 10 second
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 2) {
        std::this_thread::yield();
    }

    // Stop the device
    audioEngine.deviceStopRendering(defaultDevice);

    start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 1) {
        std::this_thread::yield();
    }
    audioEngine.deviceStartRendering(defaultDevice);
    start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 2) {
        std::this_thread::yield();
    }
    // Stop the device
    audioEngine.deviceStopRendering(defaultDevice);

    return 0;
}
