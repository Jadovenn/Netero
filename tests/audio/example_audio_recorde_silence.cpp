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

int    select_recording_device(netero::audio::engine& engine, netero::audio::device& recordDevice, netero::audio::device& silenceDevice) {
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
    recordDevice = devices[idx];
    auto renderDevice = engine.getRenderDevices();
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
    netero::audio::engine                   audioEngine;
    netero::audio::device                   recordDevice;
    netero::audio::device                   silenceDevice;

    // Select the device to record from
    if (select_recording_device(audioEngine, recordDevice, silenceDevice) != 0) {
        return 1;
    }

    // Set device error handler, to be notify if some errors happen
    netero::audio::DeviceErrorSlot          deviceErrorSlot(&errorCallback);
    try {
        auto& deviceEvent = audioEngine.getDeviceEvents(silenceDevice);
        deviceEvent.deviceErrorSig.connect(&deviceErrorSlot);
    }
    catch(...) {
        // The device is invalide
        return 1;
    }

    // If it is a loop back device play silence to make sure to record something
    if (silenceDevice) {
        audioEngine.deviceStartRendering(silenceDevice);
    }

    // Start the recording to a wave file
    netero::audio::waveRecorder             wave_recorder(audioEngine, recordDevice, std::to_string(recordDevice.format.samplingFrequency) + "hz_float32");
    audioEngine.deviceStartRecording(recordDevice);
    wave_recorder.record();

    // Wait 10 seconds
    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    
    // Stop recording
    wave_recorder.stop();
    audioEngine.deviceStopRecording(recordDevice);
    if (silenceDevice) {
        audioEngine.deviceStopRendering(silenceDevice);
    }
    return 0;
}

