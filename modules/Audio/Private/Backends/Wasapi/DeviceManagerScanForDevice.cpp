/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "DeviceManagerImpl.hpp"
#include "WasapiHelpers.hpp"

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::Impl::scanForOutputDevice()
{
    IMMDeviceCollection* pCollection = nullptr;
    UINT                 count = 0;

    HRESULT result =
        this->deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(result)) {
        goto error;
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result)) {
        goto error;
    }

    IMMDevice* pEndpoint = nullptr;
    for (ULONG idx = 0; idx < count; ++idx) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            continue;
        }
        auto it = std::find_if(this->outputDevices.begin(),
                               this->outputDevices.end(),
                               [pEndpoint](auto& device) { return *device.get() == pEndpoint; });
        if (it == this->outputDevices.end()) {
            auto devicePtr = std::make_unique<DeviceImpl>(pEndpoint, EDataFlow::eRender);
            this->clientOutputDevices.push_back(devicePtr.get());
            this->outputDevices.push_back(std::move(devicePtr));
        }
    }

    this->outputDevices.shrink_to_fit();
    this->clientOutputDevices.shrink_to_fit();

    wasapi::release<IMMDeviceCollection>(&pCollection);
    return RtCode::SUCCESS;
error:
    this->isValid = false;
    return RtCode::SYSTEM_API_ERROR;
}

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::Impl::scanForInputDevice()
{
    IMMDeviceCollection* pCollection = nullptr;
    UINT                 count = 0;

    HRESULT result =
        this->deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(result)) {
        goto error;
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result)) {
        goto error;
    }

    IMMDevice* pEndpoint = nullptr;
    for (ULONG idx = 0; idx < count; ++idx) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            continue;
        }

        auto it = std::find_if(this->inputDevices.begin(),
                               this->inputDevices.end(),
                               [pEndpoint](auto& device) { return *device.get() == pEndpoint; });
        if (it == this->inputDevices.end()) {
            auto devicePtr = std::make_unique<DeviceImpl>(pEndpoint, EDataFlow::eCapture);
            this->clientInputDevices.push_back(devicePtr.get());
            this->inputDevices.push_back(std::move(devicePtr));
        }
    }

    wasapi::release<IMMDeviceCollection>(&pCollection);

    result = this->deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(result)) {
        goto error;
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result)) {
        goto error;
    }

    pEndpoint = nullptr;
    for (ULONG idx = 0; idx < count; ++idx) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            continue;
        }

        auto it = std::find_if(this->inputDevices.begin(),
                               this->inputDevices.end(),
                               [pEndpoint](auto& device) { return *device.get() == pEndpoint; });
        if (it == this->inputDevices.end()) {
            auto devicePtr = std::make_unique<DeviceImpl>(pEndpoint, EDataFlow::eAll);
            this->clientInputDevices.push_back(devicePtr.get());
            this->inputDevices.push_back(std::move(devicePtr));
        }
    }

    wasapi::release<IMMDeviceCollection>(&pCollection);

    this->inputDevices.shrink_to_fit();
    this->clientInputDevices.shrink_to_fit();

    return RtCode::SUCCESS;
error:
    this->isValid = false;
    return RtCode::SYSTEM_API_ERROR;
}
