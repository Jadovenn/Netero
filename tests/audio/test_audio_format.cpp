/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/audio/audio.hpp>
#include <netero/audio/engine.hpp>

int     main() {
    netero::audio::engine audio_engine;

    auto outDevices = audio_engine.getRenderDevices();
    if (outDevices.empty()) {
        std::cout << "No audio devices found" << std::endl;
    }
    std::cout << "Audio output devices:" << std::endl;
    for (auto& device : outDevices) {
        std::cout << "\t" << device.name << std::endl;
        std::cout << "\t\tid: " << device.id << std::endl;
        std::cout << "\t\tmanufacturer: " << device.manufacturer << std::endl;
    }
    std::cout << std::endl;

    auto inDevices = audio_engine.getCaptureDevices();
    if (outDevices.empty()) {
        std::cout << "No in audio devices found" << std::endl;
    }
    std::cout << "Audio input devices:" << std::endl;
    for (auto& device : inDevices) {
        std::cout << "\t" << device.name << std::endl;
        std::cout << "\t\tid: " << device.id << std::endl;
        std::cout << "\t\tmanufacturer: " << device.manufacturer << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Default Output device format:" << std::endl;
    netero::audio::StreamFormat   format = audio_engine.getRenderFormat();
	std::cout << "Number of channels: " << format.channels << std::endl;
    std::cout << "Supported Sampling frequency:" << std::endl;
    for (float freq: format.supportedSamplingRate) {
    	std::cout << "\t" << freq << " Hz" << std::endl;
    }
    std::cout << "Sampling frequency: " << format.samplingFrequency << "Hz" << std::endl;
    std::cout << "Totals number of frame in buffer: " << format.framesCount << std::endl;
    std::cout << "Byte per frame: " << format.bytesPerFrame << std::endl;
    std::cout << "Byte per sample: " << format.bytesPerSample << std::endl;
    std::cout << std::endl;

    std::cout << "Default Output device format:" << std::endl;
    format = audio_engine.getCaptureFormat();
	std::cout << "Number of channels: " << format.channels << std::endl;
    std::cout << "Supported Sampling frequency:" << std::endl;
    for (float freq: format.supportedSamplingRate) {
    	std::cout << "\t" << freq << " Hz" << std::endl;
    }
    std::cout << "Sampling frequency: " << format.samplingFrequency << "Hz" << std::endl;
    std::cout << "Totals number of frame in buffer: " << format.framesCount << std::endl;
    std::cout << "Byte per frame: " << format.bytesPerFrame << std::endl;
    std::cout << "Byte per sample: " << format.bytesPerSample << std::endl;
    std::cout << std::endl;

    return 0;
}

