/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceManagerImpl.hpp"

#include <netero/os.hpp>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID   IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

netero::audio::DeviceManager::Impl::Impl()
{
    netero::os::acquireSystemResources();
    HRESULT result = CoCreateInstance(CLSID_MMDeviceEnumerator,
                                      nullptr,
                                      CLSCTX_ALL,
                                      IID_IMMDeviceEnumerator,
                                      reinterpret_cast<void**>(this->deviceEnumerator));
    // TODO: add isValid flag and check result value here
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

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::scanForDevices()
{
    return this->_pImpl->scanForOutputDevice();
}

std::vector<netero::audio::Device&>& netero::audio::DeviceManager::getOutputDevices()
{
    return this->_pImpl->clientOutputDevices;
}

std::vector<netero::audio::Device&>& netero::audio::DeviceManager::getInputDevices()
{
    return this->_pImpl->clientInputDevices;
}
