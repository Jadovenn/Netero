/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/audio/engine.hpp>

void    printDeviceAttributes(const netero::audio::device& device) {
    std::cout << device.name << std::endl;
    std::cout << "\tManufacturer: " << device.manufacturer << std::endl;
    std::cout << "\tid: " << device.id << std::endl;
	std::cout << "\t\tNumber of channels: " << device.format.channels << std::endl;
	std::cout << "\t\tSampling frequency: " << device.format.samplingFrequency << std::endl;
	std::cout << "\t\tSampling frequency: " << device.format.channels << std::endl;
    std::cout << "\t\tTotals number of frame in buffer: " << device.format.framesCount << std::endl;
    std::cout << "\t\tByte per frame: " << device.format.bytesPerFrame << std::endl;
    std::cout << "\t\tByte per sample: " << device.format.bytesPerSample << std::endl;
    std::cout << std::endl;
}

int     main() {
    auto &audioEngine = netero::audio::engine::getInstance();

    std::vector<netero::audio::device> renderDevices = audioEngine.getRenderDevices();
    if (renderDevices.empty()) {
        std::cout << "No audio devices found" << std::endl;
    }
    std::cout << "Audio render devices:" << std::endl;
    for (const netero::audio::device& device : renderDevices) {
        printDeviceAttributes(device);
    }

    std::vector<netero::audio::device> captureDevices = audioEngine.getCaptureDevices();
    if (captureDevices.empty()) {
        std::cout << "No in audio devices found" << std::endl;
    }
    std::cout << "Audio capture devices:" << std::endl;
    for (const netero::audio::device& device : captureDevices) {
        printDeviceAttributes(device);
    }
    return 0;
}

