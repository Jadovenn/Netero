/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <chrono>
#include <cmath>
#include <functional>
#include <thread>

#include <netero/audio/deviceManager.hpp>
#include <netero/netero.hpp>

int main()
{
    auto& deviceManager = netero::audio::DeviceManager::GetInstance();
    if (!deviceManager.isValid()) {
        return 1;
    }

    deviceManager.scanForDevices();
    int   defaultOutputDeviceIdx = deviceManager.getDefaultOutputDeviceIdx();
    auto& devices = deviceManager.getOutputDevices();
    int   defaultOuputDeviceIdx = deviceManager.getDefaultOutputDeviceIdx();
    if (defaultOuputDeviceIdx < 0) {
        return 1; // Could not fetch default device properly
    }
    auto* device = devices[defaultOuputDeviceIdx];

    auto  format = device->getFormat();
    float delta = 0;
    float pulsation =
        ((2 * netero::numbers::pi * 440.0) / static_cast<float>(format.samplingFrequency));
    std::function<void(float*, unsigned)> processingCallback =
        [format, pulsation, &delta](float* buffer, unsigned frames) {
            for (unsigned idx = 0; idx < frames * 2; idx += format.channels) {
                const float value = sin(pulsation * delta);
                buffer[idx] = value;
                buffer[idx + 1] = value;
                delta += 1;
            }
        };
    device->setProcessingCallback(processingCallback);

    device->open();

    auto start = std::chrono::system_clock::now();
    while (10 > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() -
                                                                 start)
                    .count()) {
        std::this_thread::yield();
    }

    device->close();

    return 0;
}
