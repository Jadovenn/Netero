/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <set>
#include <stdexcept>
#include <netero/netero.hpp>
#include <netero/graphics/context.hpp>
#include "utils/vkUtils.hpp"

namespace netero::graphics {

    std::vector<std::string> Context::getPhysicalDevices() const {
        return vkUtils::getDevicesName(this->_vulkanInstance);
    }

    std::string Context::getCurrentPhysicalDeviceName() const {
        return this->_device->deviceName;
    }

}

