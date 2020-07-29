/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/audio/deviceManager.hpp>
#include <netero/logger.hpp>

int main()
{
    auto& audioDeviceManager = netero::audio::DeviceManager::GetInstance();

    if (!audioDeviceManager.isValid()) {
        LOG_ERROR << "Audio device manager not initialized correctly!" << std::endl;
        return 1;
    }

    audioDeviceManager.scanForDevices();
    auto audioDevices = audioDeviceManager.getOutputDevices();

    LOG_INFO << "Output Devices:" << std::endl;
    for (auto device : audioDevices) {
        if (!device->isValid()) {
            LOG_ERROR << "Invalid device, system related error" << std::endl;
            continue;
        }
        LOG << '\t' << device->getName() << " (" << device->getManufacturer() << ")" << std::endl;
        auto format = device->getFormat();
        LOG << "\t\t"
            << "frequency: " << format.samplingFrequency << " Hz channels: " << format.channels
            << std::endl
            << std::endl;
    }

    audioDevices = audioDeviceManager.getInputDevices();
    LOG_INFO << "Input Devices:" << std::endl;
    for (auto device : audioDevices) {
        if (!device->isValid()) {
            LOG_ERROR << "Invalid device, system related error" << std::endl;
            continue;
        }
        LOG << '\t' << device->getName() << " (" << device->getManufacturer() << ")" << std::endl;
        auto format = device->getFormat();
        LOG << "\t\t"
            << "frequency: " << format.samplingFrequency << " Hz channels: " << format.channels
            << " isLoopback: " << device->isLoopback() << std::endl
            << std::endl;
    }
    return 0;
}
