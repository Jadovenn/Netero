/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceImpl.hpp"

#include <cstring>

netero::audio::Device::~Device() = default;

static std::string cf_string_to_std_string(CFStringRef &cf_str)
{
    int   length = CFStringGetLength(cf_str);
    char *c_str = new (std::nothrow) char[(length * 3 + 1)];
    if (!c_str) {
        throw std::bad_alloc();
    }
#if defined(UNICODE) || defined(_UNICODE)
    CFStringGetCString(cf_str, c_str, length * 3 + 1, kCFStringEncodingUTF8);
#else
    CFStringGetCString(cf_str, c_str, length * 3 + 1, CFStringGetSystemEncoding());
#endif
    std::string str(c_str);
    delete[] c_str;
    return str;
}

DeviceImpl::DeviceImpl(UInt32 id): _id(id), _isValid(true)
{
    assert(_id);

    UInt32                     size = sizeof(CFStringRef);
    AudioObjectPropertyAddress property = { kAudioObjectPropertyName,
                                            kAudioObjectPropertyScopeGlobal,
                                            kAudioObjectPropertyElementMaster };
    CFStringRef                cf_str;
    OSStatus result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, &cf_str);
    if (result != noErr) {
        this->_isValid = false;
        return;
    }
    this->_name = cf_string_to_std_string(cf_str);
    CFRelease(cf_str);

    property.mSelector = kAudioObjectPropertyManufacturer;
    result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, &cf_str);
    if (result != noErr) {
        this->_isValid = false;
        return;
    }
    this->_manufacturer = cf_string_to_std_string(cf_str);
    CFRelease(cf_str);
}

DeviceImpl::~DeviceImpl() noexcept = default;
