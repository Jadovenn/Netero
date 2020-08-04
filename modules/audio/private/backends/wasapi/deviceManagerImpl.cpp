/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceManagerImpl.hpp"

#include <netero/os.hpp>

#include "wasapiHelpers.hpp"

netero::audio::DeviceManager::Impl::Impl(): deviceEnumerator(nullptr), isValid(true)
{
    netero::os::acquireSystemResources();
    HRESULT result = CoCreateInstance(wasapi::CLSID_MMDeviceEnumerator,
                                      nullptr,
                                      CLSCTX_ALL,
                                      wasapi::IID_IMMDeviceEnumerator,
                                      reinterpret_cast<void**>(&this->deviceEnumerator));
    if (FAILED(result)) {
        this->isValid = false;
    }
}

netero::audio::DeviceManager::Impl::~Impl()
{
    netero::os::releaseSystemResources();
}

// DeviceManager Impl

netero::audio::DeviceManager::DeviceManager()
    : _pImpl(std::make_unique<netero::audio::DeviceManager::Impl>())
{
}

netero::audio::DeviceManager::~DeviceManager() = default;

bool netero::audio::DeviceManager::isValid()
{
    return this->_pImpl->isValid;
}

int netero::audio::DeviceManager::getDefaultOutputDeviceIdx()
{
    if (this->_pImpl->outputDevices.empty()) {
        return -1;
    }
    IMMDevice* deviceInterface = nullptr;
    HRESULT    result = this->_pImpl->deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
                                                                             ERole::eConsole,
                                                                             &deviceInterface);
    if (FAILED(result)) {
        return -1;
    }
    int idx = 0;
    for (auto& device : this->_pImpl->outputDevices) {
        if (*device == deviceInterface) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

int netero::audio::DeviceManager::getDefaultInputDeviceIdx()
{
    if (this->_pImpl->inputDevices.empty()) {
        return -1;
    }
    IMMDevice* deviceInterface = nullptr;
    HRESULT    result = this->_pImpl->deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture,
                                                                             ERole::eConsole,
                                                                             &deviceInterface);
    if (FAILED(result)) {
        return -1;
    }
    int idx = 0;
    for (auto& device : this->_pImpl->inputDevices) {
        if (*device == deviceInterface) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::scanForDevices()
{
    auto result = this->_pImpl->scanForOutputDevice();
    if (result != RtCode::SUCCESS) {
        return RtCode::SYSTEM_API_ERROR;
    }
    result = this->_pImpl->scanForInputDevice();
    if (result != RtCode::SUCCESS) {
        return RtCode::SYSTEM_API_ERROR;
    }
    return RtCode::SUCCESS;
}

const std::vector<netero::audio::Device*>& netero::audio::DeviceManager::getOutputDevices()
{
    return this->_pImpl->clientOutputDevices;
}

const std::vector<netero::audio::Device*>& netero::audio::DeviceManager::getInputDevices()
{
    return this->_pImpl->clientInputDevices;
}
