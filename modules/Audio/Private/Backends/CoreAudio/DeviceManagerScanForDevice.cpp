/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <memory>

#include "DeviceManagerImpl.hpp"

#include <CoreAudio/AudioHardware.h>

static UInt32 GetDeviceCount()
{
    UInt32                     size;
    AudioObjectPropertyAddress propertyAddress = { kAudioHardwarePropertyDevices,
                                                   kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMaster };
    OSStatus                   result = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
                                                     &propertyAddress,
                                                     0,
                                                     nullptr,
                                                     &size);
    if (result != noErr) {
        throw std::runtime_error("netero_audio: Error retrieving device info.");
    }
    return size / sizeof(AudioDeviceID);
}

Netero::Audio::DeviceManager::RtCode Netero::Audio::DeviceManager::Impl::ScanForDevices()
{
    UInt32 deviceCount = GetDeviceCount();
    if (deviceCount == 0) {
        return RtCode::NO_DEVICE_CONNECTED;
    }

    AudioObjectPropertyAddress property = { kAudioHardwarePropertyDevices,
                                            kAudioObjectPropertyScopeGlobal,
                                            kAudioObjectPropertyElementMaster };
    UInt32                     deviceSize = sizeof(AudioDeviceID) * deviceCount;
    AudioDeviceID              deviceIDList[deviceCount];
    OSStatus                   result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                                 &property,
                                                 0,
                                                 nullptr,
                                                 &deviceSize,
                                                 reinterpret_cast<void*>(&deviceIDList));
    if (result != noErr) {
        return RtCode::NO_DEVICE_CONNECTED;
    }

    for (auto deviceID : deviceIDList) {
        AudioBufferList* bufferList = nullptr;
        property.mSelector = kAudioDevicePropertyStreamConfiguration;
        property.mScope = kAudioDevicePropertyScopeOutput;
        property.mElement = kAudioObjectPropertyElementMaster;
        UInt32 size = 0;
        UInt32 dataSize = 0;

        result = AudioObjectGetPropertyDataSize(deviceID, &property, 0, nullptr, &dataSize);
        if (result != noErr) {
            return RtCode::SYSTEM_API_ERROR;
        }

        bufferList = new AudioBufferList[dataSize];
        result = AudioObjectGetPropertyData(deviceID, &property, 0, nullptr, &dataSize, bufferList);
        if (result != noErr) {
            delete[] bufferList;
            return RtCode::SYSTEM_API_ERROR;
        }

        unsigned channelCount = 0;
        for (unsigned i = 0; i < bufferList->mNumberBuffers; ++i)
            channelCount += bufferList->mBuffers[i].mNumberChannels;
        delete[] bufferList;

        if (channelCount > 0) {
            auto it = std::find_if(outputDevices.begin(),
                                   outputDevices.end(),
                                   [deviceID](auto& device) { return *device == deviceID; });
            if (it == outputDevices.end()) {
                auto devicePtr =
                    std::make_unique<DeviceImpl>(deviceID, kAudioObjectPropertyScopeOutput);
                clientOutputDevices.push_back(devicePtr.get());
                outputDevices.push_back(std::move(devicePtr));
            }
        }

        property.mScope = kAudioDevicePropertyScopeInput;
        result = AudioObjectGetPropertyDataSize(deviceID, &property, 0, nullptr, &dataSize);
        if (result != noErr) {
            return RtCode::SYSTEM_API_ERROR;
        }

        bufferList = new AudioBufferList[dataSize];
        result = AudioObjectGetPropertyData(deviceID, &property, 0, nullptr, &dataSize, bufferList);
        if (result != noErr) {
            delete[] bufferList;
            return RtCode::SYSTEM_API_ERROR;
        }

        channelCount = 0;
        for (unsigned i = 0; i < bufferList->mNumberBuffers; ++i)
            channelCount += bufferList->mBuffers[i].mNumberChannels;
        delete[] bufferList;

        if (channelCount > 0) {
            auto it = std::find_if(inputDevices.begin(),
                                   inputDevices.end(),
                                   [deviceID](auto& device) { return *device == deviceID; });
            if (it == inputDevices.end()) {
                auto devicePtr =
                    std::make_unique<DeviceImpl>(deviceID, kAudioObjectPropertyScopeInput);
                clientInputDevices.push_back(devicePtr.get());
                inputDevices.push_back(std::move(devicePtr));
            }
        }
    }
    outputDevices.shrink_to_fit();
    clientOutputDevices.shrink_to_fit();
    inputDevices.shrink_to_fit();
    clientInputDevices.shrink_to_fit();
    return RtCode::SUCCESS;
}
