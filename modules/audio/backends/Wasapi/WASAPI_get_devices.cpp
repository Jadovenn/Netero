/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

std::string wstring_to_string(LPCWSTR str) {
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
    if (dataFlow == DataFlow::eAll) {
        newDevice->isLoopBackDevice = true;
        newDevice->clientDevice.isLoopback = true;
    }

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

    // Get the audio session
    result = newDevice->audio_client->GetService(IID_IAudioSessionControl,
        reinterpret_cast<void**>(&newDevice->audioSession));
    if (FAILED(result)) { return nullptr; }

    // Get the rendering/capturing session
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

    // Set audio session listener
    result = newDevice->audioSession->RegisterAudioSessionNotification(newDevice.get());
    if (FAILED(result)) { return nullptr; }
    newDevice->backendDisconnectCallback = std::bind(&impl::WASAPI_disconnect_device,
        this,
        std::placeholders::_1);

	// Assign signals addresses
    newDevice->clientDevice.signals.renderStreamSig = &newDevice->renderStreamSig;
    newDevice->clientDevice.signals.captureStreamSig = &newDevice->captureStreamSig;
    newDevice->clientDevice.signals.deviceStreamFormatChangeSig = &newDevice->deviceStreamFormatChangeSig;
    newDevice->clientDevice.signals.deviceErrorSig = &newDevice->deviceErrorSig;
    newDevice->clientDevice.signals.deviceDisconnectedSig = &newDevice->deviceDisconnectedSig;
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

std::vector<netero::audio::device>    netero::audio::backend::getRenderDevices() const {
    std::vector<netero::audio::device>   devices;
    HRESULT                 result;
    IMMDeviceCollection*    pCollection = nullptr;
    IMMDevice*              pEndpoint = nullptr;
    UINT                    count = 0;

    pImpl->_garbadgeDevices.clear();
    pImpl->WASAPI_refresh_devices_list(DataFlow::eRender);
    devices.reserve(pImpl->_renderDevices.size());
    for (auto &nativeDevice : pImpl->_renderDevices) {
        devices.push_back(nativeDevice->clientDevice);
    }
    devices.shrink_to_fit();
    return devices;
    /*
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
    */
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

std::vector<netero::audio::device>   netero::audio::backend::getCaptureDevices() const {
    std::vector<netero::audio::device>   devices;
    HRESULT                 result;
    IMMDeviceCollection*    pCollection = nullptr;
    IMMDevice*              pEndpoint = nullptr;
    UINT                    count = 0;

    pImpl->_garbadgeDevices.clear();
    pImpl->WASAPI_refresh_devices_list(DataFlow::eCapture);
    pImpl->WASAPI_refresh_devices_list(DataFlow::eAll);
    devices.reserve(pImpl->_captureDevices.size());
    for (auto &nativeDevice : pImpl->_captureDevices) {
        devices.push_back(nativeDevice->clientDevice);
    }
    devices.shrink_to_fit();
    return devices;
    /*
    if (pImpl->_captureDevices.empty()) {
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
    */
}

netero::audio::device&    netero::audio::backend::getDefaultRenderDevice() const {
    (void)getRenderDevices();
    const auto defaultDeviceID = pImpl->WASAPI_get_default_device_ID(DataFlow::eRender);

    for (const auto &device : pImpl->_renderDevices) {
        if (device->clientDevice.id == defaultDeviceID) {
            return device->clientDevice;
        }
    }
    return pImpl->nullDevice;
}

netero::audio::device& netero::audio::backend::getDefaultCaptureDevice() const {
    (void)getCaptureDevices();
    const auto defaultDeviceID = pImpl->WASAPI_get_default_device_ID(DataFlow::eCapture);

    for (const auto &device : pImpl->_captureDevices) {
        if (device->clientDevice.id == defaultDeviceID) {
            return device->clientDevice;
        }
    }
    return pImpl->nullDevice;
}

// Collect disconnected device, put them in garbadge vector for latter deletion
void    netero::audio::backend::impl::WASAPI_disconnect_device(const netero::audio::device& device) {
    const auto nativeDevice = WASAPI_get_device(device);
    if (!nativeDevice) { return; } // Notify this as an error some where
    deviceDisconectedCallback(device);
    // possible data race on devices vector here
    if (nativeDevice->deviceFlow == DataFlow::eRender) {
        _renderDevices.remove(nativeDevice);
    }
    else {
        _captureDevices.remove(nativeDevice);
    }
    _garbadgeDevices.push_back(nativeDevice);
}

void    netero::audio::backend::impl::WASAPI_refresh_devices_list(DataFlow dataFlow) {
    std::vector<netero::audio::device>   devices;
    HRESULT                 result;
    IMMDeviceCollection* pCollection = nullptr;
    IMMDevice* pEndpoint = nullptr;
    UINT                    count = 0;

    if (dataFlow == DataFlow::eCapture) {
        result = device_enumerator->EnumAudioEndpoints(eCapture,
            DEVICE_STATE_ACTIVE,
            &pCollection);
        if (FAILED(result)) { return; }
    }
    else {
        result = device_enumerator->EnumAudioEndpoints(eRender,
            DEVICE_STATE_ACTIVE,
            &pCollection);
        if (FAILED(result)) { return; }
    }

    result = pCollection->GetCount(&count);
    if (FAILED(result)) {
        return;
    }
    for (ULONG idx = 0; idx < count; idx++) {
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            return;
        }
        LPWSTR deviceIDStr = WASAPI_get_device_ID(pEndpoint);
        if (!deviceIDStr) {
            WASAPI_release<IMMDevice>(&pEndpoint);
            continue;
        }
        const std::string endpointId = wstring_to_string(deviceIDStr);
        CoTaskMemFree(deviceIDStr);
        deviceIDStr = nullptr;
        if (dataFlow == DataFlow::eRender) {
            auto it = std::find_if(_renderDevices.begin(), _renderDevices.end(),
                [endpointId](std::shared_ptr<WASAPI_device> device) -> bool {
                return endpointId == device->clientDevice.id;
            });
            if (it == _renderDevices.end()) {
                auto device = WASAPI_alloc_device(pEndpoint, dataFlow);
				if (!device) {
					WASAPI_release<IMMDevice>(&pEndpoint);
					continue;
				}
                _renderDevices.push_back(device);
            }
        }
        else {
            auto it = std::find_if(_captureDevices.begin(), _captureDevices.end(),
                [endpointId](std::shared_ptr<WASAPI_device> device) -> bool {
                return endpointId == device->clientDevice.id;
            });
            if (it == _captureDevices.end()) {
                auto device = WASAPI_alloc_device(pEndpoint, dataFlow);
				if (!device) {
					WASAPI_release<IMMDevice>(&pEndpoint);
					continue;
				}
                _captureDevices.push_back(device);
            }
        }
    }
    WASAPI_release<IMMDeviceCollection>(&pCollection);
}

