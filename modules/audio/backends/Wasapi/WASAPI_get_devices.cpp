/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

std::string netero::audio::backend::impl::wstring_to_string(LPWSTR str) {
    std::string string;
    size_t converted_char = 0;
    size_t size = wcslen(str);
    size *= 2;

    if (!str) {
        return string;
    }
    auto c_string = new (std::nothrow) char[size];
    if (!c_string) {
        return string;
    }
    wcstombs_s(&converted_char, c_string, size, str, _TRUNCATE);
    string = c_string;
    delete[] c_string;
    return string;
}

std::shared_ptr<WASAPI_device>
netero::audio::backend::impl::WASAPI_alloc_device(IMMDevice* device, DataFlow dataFlow) {
    HRESULT     result;
    std::shared_ptr<WASAPI_device> newDevice = std::make_shared<WASAPI_device>();

    if (!device_enumerator || !device) {
        return nullptr;
    }
    newDevice->device = device;
    newDevice->deviceFlow = dataFlow;

    // Activate the device
    result = newDevice->device->Activate(IID_IAudioClient,
        CLSCTX_ALL,
        nullptr,
        reinterpret_cast<void**>(&newDevice->audio_client));
    if (FAILED(result)) { return nullptr; }

    // Get device stream format
    result = newDevice->audio_client->GetMixFormat(&newDevice->wfx);
    if (FAILED(result)) { return nullptr; }
    if (newDevice->wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        newDevice->wfx_ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(newDevice->wfx);
    }
    newDevice->clientDevice.format.channels = newDevice->wfx->nChannels;
    newDevice->clientDevice.format.samplingFrequency = newDevice->wfx->nSamplesPerSec;
    newDevice->clientDevice.format.bytesPerFrame = newDevice->wfx->nChannels * (newDevice->wfx->wBitsPerSample / 8);
    newDevice->clientDevice.format.bytesPerSample = newDevice->wfx->wBitsPerSample / 8;
	newDevice->clientDevice.format.supportedSamplingRate.push_back(newDevice->wfx->nSamplesPerSec);

    // Get device periode
    result = newDevice->audio_client->GetDevicePeriod(nullptr, &newDevice->latency);
    if (FAILED(result)) { return nullptr; }

    // Get device ID
    LPWSTR deviceIDStr = WASAPI_get_device_ID(newDevice->device);
    if (!deviceIDStr) { return nullptr; }
    newDevice->clientDevice.id = wstring_to_string(deviceIDStr);
    CoTaskMemFree(deviceIDStr);
    deviceIDStr = nullptr;

    // Get device name
    PROPVARIANT property = WASAPI_get_device_property(device, PKEY_Device_FriendlyName);
    if (!property.pwszVal) { PropVariantClear(&property); return nullptr; }
    newDevice->clientDevice.name = wstring_to_string(property.pwszVal);
    PropVariantClear(&property);

    // Get device manufacturer
    property = WASAPI_get_device_property(device, PKEY_DeviceInterface_FriendlyName);
    if (!property.pwszVal) { PropVariantClear(&property); return nullptr; }
    newDevice->clientDevice.manufacturer = wstring_to_string(property.pwszVal);
    PropVariantClear(&property);

    // Initialize audio client
	DWORD flags = 0;
    if (dataFlow == DataFlow::eAll) {
        flags = AUDCLNT_STREAMFLAGS_LOOPBACK;
        newDevice->clientDevice.name += " (loopback)";
    }
    result = newDevice->audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
        flags,
        0,
        0,
        newDevice->wfx,
        nullptr);
    if (FAILED(result)) { return nullptr; }

    // Get buffer size
    result = newDevice->audio_client->GetBufferSize(&newDevice->framesCount);
    if (FAILED(result)) { return nullptr; }
    newDevice->clientDevice.format.framesCount = newDevice->framesCount;

    if (dataFlow == DataFlow::eRender) {
        result = newDevice->audio_client->GetService(IID_IAudioRenderClient,
            reinterpret_cast<void**>(&newDevice->render_client));
        if (FAILED(result)) { return nullptr; }
    }
    else {
        result = newDevice->audio_client->GetService(IID_IAudioCaptureClient,
            reinterpret_cast<void**>(&newDevice->capture_client));
        if (FAILED(result)) { return nullptr; }
    }
    return newDevice;
}

LPWSTR
netero::audio::backend::impl::WASAPI_get_device_ID(IMMDevice* device) {
    HRESULT result;
    LPWSTR wsID = nullptr;

    if (!device) {
        return nullptr;
    }
    result = device->GetId(&wsID);
    if (FAILED(result)) {
        return nullptr;
    }
    return wsID;
}

PROPVARIANT
netero::audio::backend::impl::WASAPI_get_device_property(IMMDevice *device, const PROPERTYKEY propKey) {
    HRESULT result;
    LPWSTR wsID = nullptr;
    PROPVARIANT property;
    IPropertyStore* propStore = nullptr;

    PropVariantInit(&property);
    wsID = WASAPI_get_device_ID(device);
    if (!wsID) {
        return property;
    }
    result = device->OpenPropertyStore(STGM_READ, &propStore);
    if (FAILED(result)) {
        return property;
    }
    result = propStore->GetValue(propKey, &property);
    if (FAILED(result)) {
        WASAPI_release<IPropertyStore>(&propStore);
        CoTaskMemFree(wsID);
        return property;
    }
    WASAPI_release<IPropertyStore>(&propStore);
    CoTaskMemFree(wsID);
    return property;
}

const std::vector<netero::audio::device>    netero::audio::backend::getRenderDevices() {
    std::vector<netero::audio::device>   devices;
    HRESULT                 result;
    IMMDeviceCollection*    pCollection = nullptr;
    IMMDevice*              pEndpoint = nullptr;
    UINT                    count = 0;


    if (!pImpl->_renderDevices.empty()) {
        for (auto& device : pImpl->_renderDevices) {
            devices.push_back(device->clientDevice);
        }
        devices.shrink_to_fit();
        return devices;
    }
    result = pImpl->device_enumerator->EnumAudioEndpoints(eRender,
        DEVICE_STATE_ACTIVE,
        &pCollection);
    if (FAILED(result)) {
        goto exit_error;
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result) || count == 0) {
        goto exit_error;
    }
    for (ULONG idx = 0; idx < count; idx++) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            goto exit_error;
        }
        auto device = pImpl->WASAPI_alloc_device(pEndpoint, DataFlow::eRender);
        if (!device) {
            pImpl->WASAPI_release<IMMDevice>(&pEndpoint);
            continue;
        }
        pImpl->_renderDevices.push_back(device);
        devices.push_back(device->clientDevice);
    }

    pImpl->WASAPI_release<IMMDeviceCollection>(&pCollection);
    devices.shrink_to_fit();
    return devices;
exit_error:
    devices.clear();
    pImpl->_renderDevices.clear();
    devices.shrink_to_fit();
    return devices;
}

std::shared_ptr<WASAPI_device>
netero::audio::backend::impl::WASAPI_get_device(const netero::audio::device& device) {
    for (auto nativeDevice : _renderDevices) {
        if (nativeDevice->clientDevice == device) {
            return nativeDevice;
        }
    }
    for (auto nativeDevice : _captureDevices) {
        if (nativeDevice->clientDevice == device) {
            return nativeDevice;
        }
    }
    return nullptr;
}

const std::vector<netero::audio::device>   netero::audio::backend::getCaptureDevices() {
    std::vector<netero::audio::device>   devices;
    HRESULT                 result;
    IMMDeviceCollection*    pCollection = nullptr;
    IMMDevice*              pEndpoint = nullptr;
    UINT                    count = 0;


    if (!pImpl->_captureDevices.empty()) {
        for (const auto& device : pImpl->_captureDevices) {
            devices.push_back(device->clientDevice);
        }
        devices.shrink_to_fit();
        return devices;
    }
    // Get Capture devices
    result = pImpl->device_enumerator->EnumAudioEndpoints(eCapture,
        DEVICE_STATE_ACTIVE,
        &pCollection);
    if (FAILED(result)) {
        goto exit_error;
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result)) {
        goto exit_error;
    }
    for (ULONG idx = 0; idx < count; idx++) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            goto exit_error;
        }
        auto device = pImpl->WASAPI_alloc_device(pEndpoint, DataFlow::eCapture);
        if (!device) {
            pImpl->WASAPI_release<IMMDevice>(&pEndpoint);
            continue;
        }
        device->deviceFlow = DataFlow::eCapture;
        pImpl->_captureDevices.push_back(device);
        devices.push_back(device->clientDevice);
    }

    pImpl->WASAPI_release<IMMDeviceCollection>(&pCollection);

    // Get render device and setup Loopback mode
    result = pImpl->device_enumerator->EnumAudioEndpoints(eRender,
        DEVICE_STATE_ACTIVE,
        &pCollection);
    if (FAILED(result)) {
        goto exit_error;
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result) || count == 0) {
        goto exit_error;
    }
    for (ULONG idx = 0; idx < count; idx++) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            goto exit_error;
        }
        auto device = pImpl->WASAPI_alloc_device(pEndpoint, DataFlow::eAll);
        if (!device) {
            pImpl->WASAPI_release<IMMDevice>(&pEndpoint);
            continue;
        }
        pImpl->_captureDevices.push_back(device);
        devices.push_back(device->clientDevice);
    }

    pImpl->WASAPI_release<IMMDeviceCollection>(&pCollection);

    devices.shrink_to_fit();
    return devices;
exit_error:
    pImpl->WASAPI_release<IMMDeviceCollection>(&pCollection);
    devices.clear();
    pImpl->_captureDevices.clear();
    devices.shrink_to_fit();
    return devices;
}

const netero::audio::device&    netero::audio::backend::getDefaultRenderDevice() {
    (void)getRenderDevices();
    const auto defaultDeviceID = pImpl->WASAPI_get_default_device_ID(DataFlow::eRender);

    for (auto device : pImpl->_renderDevices) {
        if (device->clientDevice.id == defaultDeviceID) {
            return device->clientDevice;
        }
    }
    return pImpl->nullDevice;
}

const netero::audio::device& netero::audio::backend::getDefaultCaptureDevice() {
    (void)getCaptureDevices();
    const auto defaultDeviceID = pImpl->WASAPI_get_default_device_ID(DataFlow::eCapture);

    for (const auto device : pImpl->_captureDevices) {
        if (device->clientDevice.id == defaultDeviceID) {
            return device->clientDevice;
        }
    }
    return pImpl->nullDevice;
}
