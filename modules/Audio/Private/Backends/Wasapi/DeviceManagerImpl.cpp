/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "DeviceManagerImpl.hpp"

#include <Netero/Os.hpp>

#include "WasapiHelpers.hpp"

Netero::Audio::DeviceManager::Impl::Impl(): deviceEnumerator(nullptr), isValid(true)
{
    Netero::Os::AcquireSystemResources();
    const HRESULT result = CoCreateInstance(wasapi::CLSID_MMDeviceEnumerator,
                                            nullptr,
                                            CLSCTX_ALL,
                                            wasapi::IID_IMMDeviceEnumerator,
                                            reinterpret_cast<void**>(&this->deviceEnumerator));
    if (FAILED(result)) {
        this->isValid = false;
    }
}

Netero::Audio::DeviceManager::Impl::~Impl()
{
    Netero::Os::ReleaseSystemResources();
}

// DeviceManager Impl

Netero::Audio::DeviceManager::DeviceManager()
    : myImpl(std::make_unique<Netero::Audio::DeviceManager::Impl>())
{
}

Netero::Audio::DeviceManager::~DeviceManager() = default;

bool Netero::Audio::DeviceManager::IsValid()
{
    return myImpl->isValid;
}

int Netero::Audio::DeviceManager::GetDefaultOutputDeviceIdx()
{
    if (myImpl->outputDevices.empty()) {
        return -1;
    }
    IMMDevice* deviceInterface = nullptr;
    HRESULT    result = myImpl->deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
                                                                       ERole::eConsole,
                                                                       &deviceInterface);
    if (FAILED(result)) {
        return -1;
    }
    int idx = 0;
    for (auto& device : myImpl->outputDevices) {
        if (*device == deviceInterface) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

int Netero::Audio::DeviceManager::GetDefaultInputDeviceIdx()
{
    if (myImpl->inputDevices.empty()) {
        return -1;
    }
    IMMDevice* deviceInterface = nullptr;
    HRESULT    result = myImpl->deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture,
                                                                       ERole::eConsole,
                                                                       &deviceInterface);
    if (FAILED(result)) {
        return -1;
    }
    int idx = 0;
    for (auto& device : myImpl->inputDevices) {
        if (*device == deviceInterface) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

Netero::Audio::DeviceManager::RtCode Netero::Audio::DeviceManager::ScanForDevices()
{
    auto result = myImpl->scanForOutputDevice();
    if (result != RtCode::SUCCESS) {
        return RtCode::SYSTEM_API_ERROR;
    }
    result = myImpl->scanForInputDevice();
    if (result != RtCode::SUCCESS) {
        return RtCode::SYSTEM_API_ERROR;
    }
    return RtCode::SUCCESS;
}

const std::vector<Netero::Audio::Device*>& Netero::Audio::DeviceManager::GetOutputDevices()
{
    return myImpl->clientOutputDevices;
}

const std::vector<Netero::Audio::Device*>& Netero::Audio::DeviceManager::GetInputDevices()
{
    return myImpl->clientInputDevices;
}
