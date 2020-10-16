/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <chrono>
#include <cmath>
#include <functional>
#include <thread>

#include <Netero/Audio/DeviceManager.hpp>
#include <Netero/Netero.hpp>

int main()
{
    auto& deviceManager = Netero::Audio::DeviceManager::GetInstance();
    if (!deviceManager.IsValid()) {
        return 1;
    }

    deviceManager.ScanForDevices();
    int   defaultOutputDeviceIdx = deviceManager.GetDefaultOutputDeviceIdx();
    auto& devices = deviceManager.GetOutputDevices();
    int   defaultOuputDeviceIdx = deviceManager.GetDefaultOutputDeviceIdx();
    if (defaultOuputDeviceIdx < 0) {
        return 1; // Could not fetch default device properly
    }
    auto* device = devices[defaultOuputDeviceIdx];

    auto  format = device->GetFormat();
    float delta = 0;
    float pulsation =
        ((2 * Netero::Numbers::pi * 440.0) / static_cast<float>(format.mySamplingFrequency));
    std::function<void(float*, unsigned)> processingCallback =
        [format, pulsation, &delta](float* buffer, unsigned frames) {
            for (unsigned idx = 0; idx < frames * 2; idx += format.myChannels) {
                const float value = sin(pulsation * delta);
                buffer[idx] = value;
                buffer[idx + 1] = value;
                delta += 1;
            }
        };
    device->SetProcessingCallback(processingCallback);

    device->Open();

    auto start = std::chrono::system_clock::now();
    while (10 > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() -
                                                                 start)
                    .count()) {
        std::this_thread::yield();
    }

    device->Close();

    return 0;
}
