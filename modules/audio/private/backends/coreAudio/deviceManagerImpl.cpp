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

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::scanForDevices()
{
    return this->_pImpl->scanForOutputDevice();
}
