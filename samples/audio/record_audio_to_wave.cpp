/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

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
    auto& devices = deviceManager.getInputDevices();
    int   defaultInputDeviceIdx = deviceManager.getDefaultInputDeviceIdx();
    if (defaultInputDeviceIdx < 0) {
        return 1; // Could not fetch default device properly
    }
    auto* device = devices[0];

    auto                                        format = device->getFormat();
    std::function<void(const float*, unsigned)> acquisitionCallback = [](const float* buffer,
                                                                         unsigned     frames) {
        (void)buffer;
        (void)frames;
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

    return 0;
}
