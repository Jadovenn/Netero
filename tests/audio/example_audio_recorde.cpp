/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <netero/observer/slots.hpp>
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
    netero::audio::engine                   audio_engine;
    netero::audio::device                   device;
    netero::audio::DeviceErrorSlot          deviceErrorSlot(&errorCallback);

    if (select_recording_device(audio_engine, device) != 0) {
        return 1;
    }

    netero::audio::waveRecorder             wave_recorder(audio_engine, device, std::to_string(device.format.samplingFrequency) + "hz_float32");
    audio_engine.deviceStartRecording(device);
    wave_recorder.record();

    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    wave_recorder.stop();
    audio_engine.deviceStopRecording(device);
    return 0;
}

