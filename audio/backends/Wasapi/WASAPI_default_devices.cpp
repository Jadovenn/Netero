/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

const std::string  netero::audio::backend::impl::WASAPI_get_default_device_ID(DataFlow dataFlow) {
    HRESULT     result;
    IMMDevice*  device = nullptr;
    LPWSTR      wstrID = nullptr;
    std::string stringID;

    if (!device_enumerator) {
        return nullptr;
    }
    if (dataFlow == DataFlow::eRender) {
        result = device_enumerator->GetDefaultAudioEndpoint(eRender,
            eConsole,
            &device);
            if (FAILED(result)) { return stringID; }
    }
    else if (dataFlow == DataFlow::eCapture) {
        result = device_enumerator->GetDefaultAudioEndpoint(eCapture,
            eConsole,
            &device);
            if (FAILED(result)) { return stringID; }
    }
    else {
        return stringID;
    }
    wstrID = WASAPI_get_device_ID(device);
    if (!wstrID) { return stringID; }
    stringID = wstring_to_string(wstrID);
    CoTaskMemFree(wstrID);
    WASAPI_release<IMMDevice>(&device);
    return stringID;
}

