/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

netero::audio::RtCode
netero::audio::backend::impl::WASAPI_get_struct_Device(IMMDevice *pdevice, netero::audio::device &nt_device) {
    HRESULT result;
    LPWSTR pwszID = nullptr;
    IPropertyStore* pProps = nullptr;

    result = pdevice->GetId(&pwszID);
    if (FAILED(result)) {
        return RtCode::ERR_NATIVE;
    }
    result = pdevice->OpenPropertyStore(STGM_READ,
        &pProps);
    if (FAILED(result)) {
        return RtCode::ERR_NATIVE;
    }
    PROPVARIANT varName;
    PropVariantInit(&varName);
    result = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
    if (FAILED(result)) {
        return RtCode::ERR_NATIVE;
    }
    netero::audio::device device;
    std::wstring id(pwszID);
    std::wstring name(varName.pwszVal);
    nt_device.id = std::string(id.begin(), id.end());
    nt_device.name = std::string(name.begin(), name.end());
    PropVariantClear(&varName);

    PROPVARIANT varManufacturer;
    PropVariantInit(&varManufacturer);
    result = pProps->GetValue(PKEY_Device_Manufacturer, &varManufacturer);
    if (FAILED(result)) {
        return RtCode::ERR_NATIVE;
    }

    if (varManufacturer.pwszVal != nullptr) {
        std::wstring manufacturer(varManufacturer.pwszVal);
        nt_device.manufacturer = std::string(manufacturer.begin(), manufacturer.end());
    }
    else {
        nt_device.manufacturer = "Unknown";
    }

    PropVariantClear(&varManufacturer);
    CoTaskMemFree(pwszID);
    WASAPI_release<IPropertyStore>(&pProps);
    return RtCode::OK;
}

const std::vector<netero::audio::device>    &netero::audio::backend::impl::getOutputDevices() {
    HRESULT result;
    IMMDeviceCollection* pCollection = nullptr;
    IMMDevice* pEndpoint = nullptr;

    _outDevices.clear();
    result = _device_enumerator->EnumAudioEndpoints(eRender,
        DEVICE_STATE_ACTIVE,
        &pCollection);
    if (FAILED(result)) {
        goto exit_error;
    }

    UINT  count;
    result = pCollection->GetCount(&count);
    if (FAILED(result) || count == 0) {
        goto exit_error;
    }
    _outDevices.reserve(count);

    for (ULONG idx = 0; idx < count; idx++) {
        netero::audio::device device;
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            goto exit_error;
        }
        RtCode code = WASAPI_get_struct_Device(pEndpoint, device);
        if (code != RtCode::OK) {
            goto exit_error;
        }
        _outDevices.push_back(device);
        WASAPI_release<IMMDevice>(&pEndpoint);
    }

    return _outDevices;
exit_error:
    _outDevices.clear();
    WASAPI_release<IMMDeviceCollection>(&pCollection);
    WASAPI_release<IMMDevice>(&pEndpoint);
    return _outDevices;
}

const std::vector<netero::audio::device>   &netero::audio::backend::impl::getInputDevices() {
    HRESULT result;
    IMMDeviceCollection* pCollection = nullptr;
    IMMDevice* pEndpoint = nullptr;

    _inDevices.clear();
    result = _device_enumerator->EnumAudioEndpoints(eCapture,
        DEVICE_STATE_ACTIVE,
        &pCollection);
    if (FAILED(result)) {
        goto exit_error;
    }

    UINT  count;
    result = pCollection->GetCount(&count);
    if (FAILED(result)) {
        goto exit_error;
    }
    _outDevices.reserve(count);
   
    for (ULONG idx = 0; idx < count; idx++) {
        netero::audio::device device;
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            goto exit_error;
        }
        RtCode code = WASAPI_get_struct_Device(pEndpoint, device);
        if (code != RtCode::OK) {
            goto exit_error;
        }
        _inDevices.push_back(device);
        WASAPI_release<IMMDevice>(&pEndpoint);
    }

    return _inDevices;
exit_error:
    _outDevices.clear();
    WASAPI_release<IMMDeviceCollection>(&pCollection);
    WASAPI_release<IMMDevice>(&pEndpoint);
    return _inDevices;
}

