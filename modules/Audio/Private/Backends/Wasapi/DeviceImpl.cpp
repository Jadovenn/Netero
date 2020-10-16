/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "DeviceImpl.hpp"

#include <cassert>

#include <Functiondiscoverykeys_devpkey.h>

netero::audio::Device::~Device() = default;

DeviceImpl::DeviceImpl(IMMDevice* aDeviceInterface, EDataFlow aDataFlow)
    : _renderingAsyncState(AsyncState::STOP),
      _acquisitionAsyncState(AsyncState::STOP),
      _isValid(true),
      _isOpen(false),
      _deviceIdentifier(aDeviceInterface),
      _dataflow(aDataFlow),
      _audioClient(nullptr),
      _wfx(nullptr)
{
    assert(this->_deviceIdentifier);

    HRESULT result =
        this->_deviceIdentifier->Activate(wasapi::IID_IAudioClient,
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
        wasapi::GetDeviceProperty(this->_deviceIdentifier, PKEY_Device_FriendlyName);
    if (!property.pwszVal) {
        this->_isValid = false;
        return;
    }
    this->_name = wasapi::wstring_to_string(property.pwszVal);
    PropVariantClear(&property);
    if (this->_dataflow == EDataFlow::eAll) {
        this->_name += " (loopback)";
    }

    property =
        wasapi::GetDeviceProperty(this->_deviceIdentifier, PKEY_DeviceInterface_FriendlyName);
    if (!property.pwszVal) {
        this->_isValid = false;
        return;
    }
    this->_manufacturer = wasapi::wstring_to_string(property.pwszVal);
    PropVariantClear(&property);
}

DeviceImpl::~DeviceImpl()
{
    wasapi::release<IMMDevice>(&this->_deviceIdentifier);
    wasapi::release<IAudioClient>(&this->_audioClient);
    if (this->_wfx) {
        CoTaskMemFree(this->_wfx);
    }
}

netero::audio::Device::RtCode DeviceImpl::open()
{
    if (this->_isOpen) {
        return RtCode::ALREADY_OPEN;
    }
    if (!this->_isValid) {
        return RtCode::SYSTEM_ERROR;
    }
    if (this->_dataflow == EDataFlow::eRender) {
        if (!this->_processingCallback) {
            return RtCode::NO_REGISTERED_CALLBACK;
        }
        return this->openForRendering();
    }
    else if (this->_dataflow == EDataFlow::eCapture || this->_dataflow == EDataFlow::eAll) {
        if (!this->_acquisitionCallback) {
            return RtCode::NO_REGISTERED_CALLBACK;
        }
        return this->openForAcquisition();
    }
    return RtCode::SYSTEM_ERROR;
}

netero::audio::Device::RtCode DeviceImpl::close()
{
    if (!this->_isOpen) {
        return RtCode::NOT_OPEN;
    }
    if (!this->_isValid) {
        return RtCode::SYSTEM_ERROR;
    }
    if (this->_dataflow == EDataFlow::eRender) {
        return this->closeAfterRendering();
    }
    else if (this->_dataflow == EDataFlow::eCapture || this->_dataflow == EDataFlow::eAll) {
        return this->closeAfterAcquisition();
    }
    return RtCode::SYSTEM_ERROR;
}
