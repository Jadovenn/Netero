/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Audio/DeviceManager.hpp>
#include <Netero/Logger.hpp>

int main()
{
    auto& audioDeviceManager = Netero::Audio::DeviceManager::GetInstance();

    if (!audioDeviceManager.IsValid()) {
        LOG_ERROR << "Audio device manager not initialized correctly!" << std::endl;
        return 1;
    }

    audioDeviceManager.ScanForDevices();
    auto audioDevices = audioDeviceManager.GetOutputDevices();
    int  defaultOuputDeviceIdx = audioDeviceManager.GetDefaultOutputDeviceIdx();

    LOG_INFO << "Output Devices:" << std::endl;
    if (defaultOuputDeviceIdx > 0) {
        LOG << "Default Device: " << audioDevices[defaultOuputDeviceIdx]->GetName() << std::endl;
    }
    for (auto device : audioDevices) {
        if (!device->IsValid()) {
            LOG_ERROR << "Invalid device, system related error" << std::endl;
            continue;
        }
        LOG << '\t' << device->GetName() << " (" << device->GetManufacturer() << ")" << std::endl;
        auto format = device->GetFormat();
        LOG << "\t\t"
            << "frequency: " << format.mySamplingFrequency << " Hz channels: " << format.myChannels
            << std::endl
            << std::endl;
    }

    audioDevices = audioDeviceManager.GetInputDevices();
    int defaultInputDeviceIdx = audioDeviceManager.GetDefaultInputDeviceIdx();

    LOG_INFO << "Input Devices:" << std::endl;
    if (defaultInputDeviceIdx >= 0) {
        LOG << "Default Device: " << audioDevices[defaultInputDeviceIdx]->GetName() << std::endl;
    }
    for (auto device : audioDevices) {
        if (!device->IsValid()) {
            LOG_ERROR << "Invalid device, system related error" << std::endl;
            continue;
        }
        LOG << '\t' << device->GetName() << " (" << device->GetManufacturer() << ")" << std::endl;
        auto format = device->GetFormat();
        LOG << "\t\t"
            << "frequency: " << format.mySamplingFrequency << " Hz channels: " << format.myChannels
            << " isLoopback: " << device->IsLoopback() << std::endl
            << std::endl;
    }
    return 0;
}
