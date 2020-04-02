/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <netero/observer/slots.hpp>
#include <netero/audio/audio.hpp>
#include <netero/audio/engine.hpp>
#include <netero/audio/waveFormat.hpp>

void    errorCallback(const std::string& error) {
    std::cout << "[ERROR] " << error << std::endl;
}

int    select_recording_device(netero::audio::engine& engine) {
    auto devices = engine.getOutDevices();
    auto device_in = engine.getInDevices();
    devices.insert(devices.end(), device_in.begin(), device_in.end());
    int idx = 0;
    for (auto& device : devices) {
        std::cout << idx << ": " << device.name << std::endl;
        idx += 1;
    }
    std::cout << "Choose a device: ";
    int device_idx = -1;
    std::cin >> device_idx;
    if (device_idx < 0 || device_idx > idx) {
        std::cout << "Choosen idx out of bound!" << std::endl;
        return 1;
    }
    netero::audio::RtCode code = engine.setOutputDevice(devices[device_idx]);
    if (code != netero::audio::RtCode::OK) {
        std::cout << "Error while setting up the device: " << devices[device_idx].name << std::endl;
        return 1;
    }
    return 0;
}

int     main() {
    netero::audio::engine                   audio_engine;
    netero::audio::waveRecorder             wave_recorder("test_record");
    netero::slot<void(const std::string&)>  captureErrorSlot(&errorCallback);

    if (select_recording_device(audio_engine) != 0) {
        return 1;
    }
    audio_engine.captureErrorSig.connect(&captureErrorSlot);
    wave_recorder.connect(audio_engine);
    audio_engine.startCapture();
    wave_recorder.record();
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    wave_recorder.stop();
    audio_engine.stopCapture();
    return 0;
}

