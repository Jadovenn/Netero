/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <netero/observer/slot.hpp>
#include <netero/audio/format/waveRecorder.hpp>

void    errorCallback(const std::string& message) {
    std::cout << message << std::endl;
}

int    select_recording_device(netero::audio::engine& engine, netero::audio::device &device) {
    auto devices = engine.getCaptureDevices();
    int counter = 0;
    for (auto& device : devices) {
        std::cout << counter << ": " << device.name << std::endl;
        counter += 1;
    }
    std::cout << "Choose a device: ";
    int idx = -1;
    std::cin >> idx;
    if (idx < 0 || idx > counter) {
        std::cout << "Choosen idx out of bound!" << std::endl;
        return 1;
    }
    device = devices[idx];
    std::cout << "Number of channels: " << device.format.channels << std::endl;
    std::cout << "Supported Sampling frequency:" << std::endl;
    for (float freq : device.format.supportedSamplingRate) {
        std::cout << "\t" << freq << " Hz" << std::endl;
    }
    std::cout << "Sampling frequency: " << device.format.samplingFrequency << "Hz" << std::endl;
    std::cout << "Totals number of frame in buffer: " << device.format.framesCount << std::endl;
    std::cout << "Byte per frame: " << device.format.bytesPerFrame << std::endl;
    std::cout << "Byte per sample: " << device.format.bytesPerSample << std::endl;
    std::cout << std::endl;
    return 0;
}

int     main() {
    auto &audioEngine = netero::audio::engine::getInstance();
    netero::audio::device                   device;
    netero::audio::DeviceErrorSlot          deviceErrorSlot(&errorCallback);

    if (select_recording_device(audioEngine, device) != 0) {
        return 1;
    }

    // Record 1
    auto *waveRecorder = new netero::audio::waveRecorder(audioEngine, device, std::to_string(device.format.samplingFrequency) + "1-hz_float32");
    audioEngine.deviceStartRecording(device);
    waveRecorder->record();

    auto start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    waveRecorder->stop();
    audioEngine.deviceStopRecording(device);
    delete waveRecorder;

    // Record 2
    waveRecorder = new netero::audio::waveRecorder(audioEngine, device, std::to_string(device.format.samplingFrequency) + "2-hz_float32");
    audioEngine.deviceStartRecording(device);
    waveRecorder->record();

    start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    waveRecorder->stop();
    audioEngine.deviceStopRecording(device);
    delete waveRecorder;

    auto devices = audioEngine.getCaptureDevices();
    for (auto& deviceItem : devices) {
        std::cout << ": " << deviceItem.name << std::endl;
    }
    return 0;
}

