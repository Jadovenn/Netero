/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceImpl.hpp"

#include <cassert>

#include "wasapiHelpers.hpp"

#include <Functiondiscoverykeys_devpkey.h>

DeviceImpl::DeviceImpl(IMMDevice* aDeviceInterface)
    : _deviceInterface(aDeviceInterface), _isValid(true)
{
    assert(this->_deviceInterface);

    PROPVARIANT property =
        wasapi::GetDeviceProperty(this->_deviceInterface, PKEY_Device_FriendlyName);
    if (!property.pwszVal) {
        this->_isValid = false;
        return;
    }
    this->_name = wasapi::wstring_to_string(property.pwszVal);
    PropVariantClear(&property);

    property = wasapi::GetDeviceProperty(this->_deviceInterface, PKEY_DeviceInterface_FriendlyName);
    if (!property.pwszVal) {
        this->_isValid = false;
        return;
    }
    this->_manufacturer = wasapi::wstring_to_string(property.pwszVal);
    PropVariantClear(&property);
}

DeviceImpl::~DeviceImpl()
{
}
