/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <type_traits>

#include <mmdeviceapi.h>
#include <windows.h>

namespace wasapi {

extern const CLSID CLSID_MMDeviceEnumerator;
extern const IID   IID_IMMDeviceEnumerator;
extern const IID   IID_IAudioClient;
extern const IID   IID_IAudioRenderClient;
extern const IID   IID_IAudioCaptureClient;
extern const IID   IID_IAudioSessionControl;

template<class T, typename = std::enable_if<std::is_base_of<IUnknown, T>::value>>
static inline void release(T** ptr)
{
    if (*ptr) {
        (*ptr)->Release();
        *ptr = nullptr;
    }
}

std::string wstring_to_string(LPCWSTR str);
LPWSTR      GetDeviceID(IMMDevice*);
PROPVARIANT GetDeviceProperty(IMMDevice*, const PROPERTYKEY);

} // namespace wasapi
