/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceManagerImpl.hpp"

netero::audio::DeviceManager::Impl::Impl()
{
}

netero::audio::DeviceManager::Impl::~Impl()
{
}

// DeviceManager Impl

netero::audio::DeviceManager::DeviceManager()
    : _pImpl(std::make_unique<netero::audio::DeviceManager::Impl>())
{
}

netero::audio::DeviceManager::~DeviceManager() noexcept = default;

bool netero::audio::DeviceManager::isValid() {
    return true;
}

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::scanForDevices()
{
    return this->_pImpl->scanForDevices();
}

const std::vector<netero::audio::Device*>& netero::audio::DeviceManager::getOutputDevices()
{
    return this->_pImpl->clientOutputDevices;
}

const std::vector<netero::audio::Device*>& netero::audio::DeviceManager::getInputDevices()
{
    return this->_pImpl->clientInputDevices;
}
