/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <thread>

#include <netero/audio/deviceManager.hpp>
#include <netero/audio/waveFile.hpp>
#include <netero/logger.hpp>

int ChooseDevice()
{
    auto& deviceManager = netero::audio::DeviceManager::GetInstance();
    deviceManager.scanForDevices();
    auto& devices = deviceManager.getInputDevices();

    int defaultInputDeviceIdx = deviceManager.getDefaultInputDeviceIdx();
    LOG << "Choose a Device:" << std::endl;
    if (defaultInputDeviceIdx >= 0) {
        LOG << "Default input device: " << devices[defaultInputDeviceIdx]->getName() << std::endl;
    }
    int idx = 0;
    for (auto& device : devices) {
        LOG << idx << ": " << device->getName() << std::endl;
        idx += 1;
    }

    LOG << "idx ? => ";
    std::cin >> idx;
    return idx;
}

int main()
{
    auto& deviceManager = netero::audio::DeviceManager::GetInstance();

    int idx = ChooseDevice();
    deviceManager.scanForDevices();
    auto& devices = deviceManager.getInputDevices();

    if (idx < 0 || idx >= devices.size()) {
        LOG << "Idx out of range you try to trick me or devices have changed" << std::endl;
        return 1;
    }

    auto&                   device = devices[idx];
    netero::audio::WaveFile saveFile;

    const auto& format = device->getFormat();
    saveFile.setFormat(format);
    std::string fileName;
    fileName += std::to_string(format.channels);
    fileName += "channels_";
    fileName += std::to_string(format.samplingFrequency);
    fileName += "Hz.wav";
    saveFile.open(fileName, std::fstream::out | std::fstream::trunc | std::fstream::binary);

    if (!saveFile.is_open()) {
        LOG << "Failed to create " << fileName << "file." << std::endl;
        return 1;
    }

    std::function<void(const float*, unsigned)> acquisitionCallback =
        [&saveFile, format](const float* buffer, unsigned frames) {
            saveFile.write(buffer, frames * format.channels);
        };

    device->setAcquisitionCallback(acquisitionCallback);
    device->open();
    auto start = std::chrono::system_clock::now();
    while (10 > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() -
                                                                 start)
                    .count()) {
        std::this_thread::yield();
    }

    device->close();
    saveFile.close();

    return 0;
}
