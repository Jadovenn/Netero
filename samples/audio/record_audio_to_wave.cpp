/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <thread>

#include <Netero/Audio/DeviceManager.hpp>
#include <Netero/Audio/WaveFile.hpp>
#include <Netero/Logger.hpp>

int ChooseDevice()
{
    auto& deviceManager = Netero::Audio::DeviceManager::GetInstance();
    deviceManager.ScanForDevices();
    auto& devices = deviceManager.GetInputDevices();

    int defaultInputDeviceIdx = deviceManager.GetDefaultInputDeviceIdx();
    LOG << "Choose a Device:" << std::endl;
    if (defaultInputDeviceIdx >= 0) {
        LOG << "Default input device: " << devices[defaultInputDeviceIdx]->GetName() << std::endl;
    }
    int idx = 0;
    for (auto& device : devices) {
        LOG << idx << ": " << device->GetName() << std::endl;
        idx += 1;
    }

    LOG << "idx ? => ";
    std::cin >> idx;
    return idx;
}

int main()
{
    auto& deviceManager = Netero::Audio::DeviceManager::GetInstance();

    int idx = ChooseDevice();
    deviceManager.ScanForDevices();
    auto& devices = deviceManager.GetInputDevices();

    if (idx < 0 || idx >= devices.size()) {
        LOG << "Idx out of range you try to trick me or devices have changed" << std::endl;
        return 1;
    }

    auto&                   device = devices[idx];
    Netero::Audio::WaveFile saveFile;

    const auto& format = device->GetFormat();
    saveFile.SetFormat(format);
    std::string fileName;
    fileName += std::to_string(format.myChannels);
    fileName += "channels_";
    fileName += std::to_string(format.mySamplingFrequency);
    fileName += "Hz.wav";
    saveFile.Open(fileName, std::fstream::out | std::fstream::trunc | std::fstream::binary);

    if (!saveFile.IsOpen()) {
        LOG << "Failed to create " << fileName << "file." << std::endl;
        return 1;
    }

    std::function<void(const float*, unsigned)> acquisitionCallback =
        [&saveFile, format](const float* buffer, unsigned frames) {
            saveFile.Write(buffer, frames * format.myChannels);
        };

    device->SetAcquisitionCallback(acquisitionCallback);
    device->Open();
    auto start = std::chrono::system_clock::now();
    while (10 > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() -
                                                                 start)
                    .count()) {
        std::this_thread::yield();
    }

    device->Close();
    saveFile.Close();

    return 0;
}
