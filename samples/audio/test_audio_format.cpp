/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/audio/deviceManager.hpp>

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
    auto &deviceManager = netero::audio::DeviceManager::getInstance();

    auto renderDevices = deviceManager.getRenderDevices();
    if (renderDevices.empty()) {
        std::cout << "No audio devices found" << std::endl;
    }
    std::cout << "Audio render devices:" << std::endl;
    for (const auto& device : renderDevices) {
        printDeviceAttributes(device);
    }

    std::vector<netero::audio::device> captureDevices = deviceManager.getCaptureDevices();
    if (captureDevices.empty()) {
        std::cout << "No in audio devices found" << std::endl;
    }
    std::cout << "Audio capture devices:" << std::endl;
    for (const auto& device : captureDevices) {
        printDeviceAttributes(device);
    }
    return 0;
}

