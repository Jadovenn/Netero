/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <netero/observer/slot.hpp>
#include <netero/audio/engine.hpp>
#include <netero/audio/deviceManager.hpp>
#include <netero/audio/format/waveRecorder.hpp>

void    errorCallback(const std::string& message) {
    std::cout << message << std::endl;
}

int    select_recording_device(netero::audio::device& recordDevice, netero::audio::device& silenceDevice) {
    auto devices = netero::audio::DeviceManager::getInstance().getCaptureDevices();
    int counter = 0;
    for (auto& device : devices) {
        std::cout << counter << ": " << device.name << std::endl;
        counter += 1;
    }
    std::cout << "Choose a device: ";
    int idx = -1;
    std::cin >> idx;
    if (idx < 0 || idx > counter) {
        std::cout << "Chosen idx out of bound!" << std::endl;
        return 1;
    }
    recordDevice = devices[idx];
    auto renderDevice =  netero::audio::DeviceManager::getInstance().getRenderDevices();
    for (auto& device : renderDevice) {
        if (device.id == recordDevice.id) {
            silenceDevice = device;
        }
    }
    std::cout << "Number of channels: " << recordDevice.format.channels << std::endl;
    std::cout << "Supported Sampling frequency:" << std::endl;
    for (float freq : recordDevice.format.supportedSamplingRate) {
        std::cout << "\t" << freq << " Hz" << std::endl;
    }
    std::cout << "Sampling frequency: " << recordDevice.format.samplingFrequency << "Hz" << std::endl;
    std::cout << "Totals number of frame in buffer: " << recordDevice.format.framesCount << std::endl;
    std::cout << "Byte per frame: " << recordDevice.format.bytesPerFrame << std::endl;
    std::cout << "Byte per sample: " << recordDevice.format.bytesPerSample << std::endl;
    std::cout << std::endl;
    return 0;
}

int     main() {
    auto& audioEngine = netero::audio::engine::getInstance();
    netero::audio::device                   recordDevice;
    netero::audio::device                   silenceDevice;

    // Select the device to record from
    if (select_recording_device(recordDevice, silenceDevice) != 0) {
        return 1;
    }
	if (silenceDevice) {
		audioEngine.setRenderDevice(silenceDevice);
	}
    audioEngine.setCaptureDevice(recordDevice);

    // Set device error handler, to be notify if some errors happen
    netero::audio::DeviceErrorSlot          deviceErrorSlot(&errorCallback);
    silenceDevice.signals.deviceErrorSig->connect(&deviceErrorSlot);

    // Start the recording to a wave file
    auto* waveRecorder = audioEngine.createCaptureEntity<netero::audio::waveRecorder>("48kHz_stereo");
    waveRecorder->record();

    // Wait 10 seconds
    const auto start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    
    // Stop recording
    waveRecorder->stop();
    audioEngine.disconnectCaptureDevice();
    audioEngine.disconnectRenderDevice();
    return 0;
}

