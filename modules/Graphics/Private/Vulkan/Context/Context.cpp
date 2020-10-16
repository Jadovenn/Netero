/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Vulkan/Context/Context.hpp>

namespace Netero::Gfx {

Context::Context()
    : myVulkanInstance(nullptr),
      mySurface(nullptr),
      myWidth(0),
      myHeight(0),
      myPhysicalDevice(nullptr),
      myLogicalDevice(nullptr),
      myGraphicsQueue(nullptr),
      myPresentQueue(nullptr),
      myTransferQueue(nullptr),
      myTransferCommandPool(nullptr)
{
}

} // namespace Netero::Gfx
