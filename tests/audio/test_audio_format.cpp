/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>

#include <netero/audio/audio.hpp>
#include <netero/audio/device.hpp>

int     main() {
    netero::audio::device& audio_device = netero::audio::device::GetAudioDevice();
    netero::audio::WaveFormat   format = audio_device.getWaveFormat();
    std::cout << "Device name: " << format.name << std::endl;
	std::cout << "Device manufacturer: " << format.manufacturer << std::endl;
	std::cout << "Number of channels: " << format.channels << std::endl;
    std::cout << "Supported Sampling frequency:" << std::endl;
    for (float freq: format.supportedSamplingRate) {
    	std::cout << "\t" << freq << " Hz" << std::endl;
    }
    std::cout << "Sampling frequency: " << format.samplingFrequency << "Hz" << std::endl;
    std::cout << "Totals number of frame in buffer: " << format.framesCount << std::endl;
    std::cout << "Byte per frame: " << format.bytesPerFrame << std::endl;
    std::cout << "Byte per sample: " << format.bytesPerSample << std::endl;
    return 0;
}

