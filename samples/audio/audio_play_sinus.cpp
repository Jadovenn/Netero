/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

int main()
{
    return 0;
}

/**
#include <chrono>
#include <iostream>
#include <thread>

#include <netero/audio/deviceManager.hpp>
#include <netero/audio/engine.hpp>
#include <netero/audio/signals.hpp>

static 
void    deviceDisconnectionHandler(const netero::audio::device &device) {
    std::cout << "device: " << device.name << " has been disconnected." << std::endl;
}

static
netero::slot<void(const netero::audio::device&)>    deviceDisconnectionSlot(&deviceDisconnectionHandler);

int main() {
    try {
        // Get the audio engine instance
        auto& audioEngine = netero::audio::engine::getInstance();
        netero::audio::DeviceManager::getInstance().deviceDisconnectedSig.connect(&deviceDisconnectionSlot);

    	// Create 3 audio entities, which are sinusoid signal
        auto* noteDo = audioEngine.createRenderEntity<netero::audio::signals::sinusoid>(0.01F, 261.63F, 0.0F);
        auto* noteMi = audioEngine.createRenderEntity<netero::audio::signals::sinusoid>(0.01F, 329.63F, 0.0F);
        auto* noteLa = audioEngine.createRenderEntity<netero::audio::signals::sinusoid>(0.01F, 400.0F, 0.0F);

        noteDo->play();

        auto start = std::chrono::system_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 2) {
            std::this_thread::yield();
        }

        noteMi->play();
        noteDo->stop();

        start = std::chrono::system_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 2) {
            std::this_thread::yield();
        }

        noteLa->play();
        noteMi->stop();

        start = std::chrono::system_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 2) {
            std::this_thread::yield();
        }

        noteDo->play();
        noteMi->play();

    	start = std::chrono::system_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 4) {
            std::this_thread::yield();
        }

    	// Explicitly release audio entity, they also might be deleted by the engine
    	// while the process exit
        audioEngine.releaseRenderEntity(noteLa);
        audioEngine.releaseRenderEntity(noteDo);
        audioEngine.releaseRenderEntity(noteMi);
    }
	catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
	}
	catch (...) {
        std::cout << "Unexpected exception." << std::endl;
	}
    return 0;
}
*/