/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceImpl.hpp"

#include <cassert>

#include "wasapiHelpers.hpp"

#include <Functiondiscoverykeys_devpkey.h>

netero::audio::Device::~Device() = default;

DeviceImpl::DeviceImpl(IMMDevice* aDeviceInterface, EDataFlow aDataFlow)
    : _isValid(true),
      _deviceInterface(aDeviceInterface),
      _dataflow(aDataFlow),
      _audioClient(nullptr),
      _wfx(nullptr)
{
    assert(this->_deviceInterface);

    HRESULT result =
        this->_deviceInterface->Activate(wasapi::IID_IAudioClient,
                                         CLSCTX_ALL,
                                         nullptr,
                                         reinterpret_cast<void**>(&this->_audioClient));
    if (FAILED(result)) {
        this->_isValid = false;
        return;
    }

    result = this->_audioClient->GetMixFormat(&this->_wfx);
    if (FAILED(result)) {
        this->_isValid = false;
        return;
    }
    this->_deviceFomat.channels = this->_wfx->nChannels;
    this->_deviceFomat.samplingFrequency = this->_wfx->nSamplesPerSec;
    this->_deviceFomat.bytesPerFrame = this->_wfx->nChannels * (this->_wfx->wBitsPerSample / 8);
    this->_deviceFomat.bytesPerSample = this->_wfx->wBitsPerSample / 8;
    this->_deviceFomat.supportedSamplingRate.push_back(this->_wfx->nSamplesPerSec);

    PROPVARIANT property =
        wasapi::GetDeviceProperty(this->_deviceInterface, PKEY_Device_FriendlyName);
    if (!property.pwszVal) {
        this->_isValid = false;
        return;
    }
    this->_name = wasapi::wstring_to_string(property.pwszVal);
    PropVariantClear(&property);
    if (this->_dataflow == EDataFlow::eAll) {
        this->_name += " (loopback)";
    }

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
    wasapi::release<IMMDevice>(&this->_deviceInterface);
    wasapi::release<IAudioClient>(&this->_audioClient);
    if (this->_wfx) {
        CoTaskMemFree(this->_wfx);
    }
}
