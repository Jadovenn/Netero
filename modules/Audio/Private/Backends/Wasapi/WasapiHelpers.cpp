/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "WasapiHelpers.hpp"

#include <AudioClient.h>
#include <audiopolicy.h>

namespace wasapi {

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID   IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID   IID_IAudioClient = __uuidof(IAudioClient);
const IID   IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID   IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID   IID_IAudioSessionControl = __uuidof(IAudioSessionControl);

std::string wstring_to_string(LPCWSTR str)
{
    std::string string;
    size_t      converted_char = 0;
    size_t      size = wcslen(str);
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

LPWSTR GetDeviceID(IMMDevice* device)
{
    HRESULT result;
    LPWSTR  wsID = nullptr;

    if (!device) {
        return nullptr;
    }
    result = device->GetId(&wsID);
    if (FAILED(result)) {
        return nullptr;
    }
    return wsID;
}

PROPVARIANT GetDeviceProperty(IMMDevice* device, const PROPERTYKEY propKey)
{
    HRESULT         result;
    LPWSTR          wsID = nullptr;
    PROPVARIANT     property;
    IPropertyStore* propStore = nullptr;

    PropVariantInit(&property);
    wsID = wasapi::GetDeviceID(device);
    if (!wsID) {
        return property;
    }
    result = device->OpenPropertyStore(STGM_READ, &propStore);
    if (FAILED(result)) {
        return property;
    }
    result = propStore->GetValue(propKey, &property);
    if (FAILED(result)) {
        wasapi::release<IPropertyStore>(&propStore);
        CoTaskMemFree(wsID);
        return property;
    }
    wasapi::release<IPropertyStore>(&propStore);
    CoTaskMemFree(wsID);
    return property;
}

} // namespace wasapi
