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
    netero::audio::engine                   audio_engine;
    netero::audio::device                   recordDevice;
    netero::audio::device                   silenceDevice;



    if (select_recording_device(audio_engine, recordDevice, silenceDevice) != 0) {
        return 1;
    }
    // Set device error handler, to be notify if some errors happened
    netero::audio::DeviceErrorSlot          deviceErrorSlot(&errorCallback);
    try {
        auto& deviceEvent = audio_engine.getDeviceEvents(silenceDevice);
        deviceEvent.deviceErrorSig.connect(&deviceErrorSlot);
    }
    catch(...) {}

    netero::audio::waveRecorder             wave_recorder(audio_engine, recordDevice, std::to_string(recordDevice.format.samplingFrequency) + "hz_float32");
    if (silenceDevice) {
        audio_engine.deviceStartRendering(silenceDevice);
    }
    audio_engine.deviceStartRecording(recordDevice);
    wave_recorder.record();

    std::chrono::time_point start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < 10) {
        std::this_thread::yield();
    }
    wave_recorder.stop();
    audio_engine.deviceStopRecording(recordDevice);
    if (silenceDevice) {
        audio_engine.deviceStopRendering(silenceDevice);
    }
    return 0;
}
