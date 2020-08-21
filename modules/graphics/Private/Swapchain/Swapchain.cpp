/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>

#include <Vulkan/Context/Context.hpp>
#include <Vulkan/VkUtils.hpp>

#include <Swapchain/Swapchain.hpp>

namespace Netero::Gfx {

Swapchain::Swapchain(Context& aContext, uint32_t aMaxInFlightFrameCount)
    : myContext(aContext),
      myCurrentFrame(0),
      myFrameCount(0),
      myMaximumInFligthFrames(aMaxInFlightFrameCount),
      mySwapchain(nullptr),
      mySwapchainExtent { 0, 0 },
      mySwapchainImageFormat(VK_FORMAT_UNDEFINED)
{
    this->myAvailableFrameSemaphore.resize(myMaximumInFligthFrames);
    this->myRenderedFrameSemaphore.resize(myMaximumInFligthFrames);
    this->myInFlyFrameFence.resize(myMaximumInFligthFrames);
    this->myRenderedFrameFence.resize(myMaximumInFligthFrames);
}

Swapchain::RtCode Swapchain::Initialize()
{
    assert(myContext.myLogicalDevice != nullptr);  // Logical must be ready at this point
    assert(myContext.myPhysicalDevice != nullptr); // Physical must be ready at this point
    assert(myContext.mySurface != nullptr);        // Surface must be ready at this point
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (unsigned idx = 0; idx < myMaximumInFligthFrames; idx++) {
        VkResult result = vkCreateSemaphore(myContext.myLogicalDevice,
                                            &semaphoreInfo,
                                            nullptr,
                                            &myAvailableFrameSemaphore[idx]);
        if (result != VK_SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
        result = vkCreateSemaphore(myContext.myLogicalDevice,
                                   &semaphoreInfo,
                                   nullptr,
                                   &myRenderedFrameSemaphore[idx]);
        if (result != VK_SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
        result =
            vkCreateFence(myContext.myLogicalDevice, &fenceInfo, nullptr, &myInFlyFrameFence[idx]);
        if (result != VK_SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
    }
    CreateSwapchain();
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::Teardown()
{
    Release();
    for (int idx = 0; idx < myMaximumInFligthFrames; idx++) {
        vkDestroySemaphore(myContext.myLogicalDevice, myAvailableFrameSemaphore[idx], nullptr);
        vkDestroySemaphore(myContext.myLogicalDevice, myRenderedFrameSemaphore[idx], nullptr);
        vkDestroyFence(myContext.myLogicalDevice, myInFlyFrameFence[idx], nullptr);
    }
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::Build()
{
    CreateImageViews();
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::ReBuild()
{
    Release();
    CreateSwapchain();
    CreateImageViews();
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::Release()
{
    for (auto* imageView : mySwapchainImageViews) {
        vkDestroyImageView(myContext.myLogicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(myContext.myLogicalDevice, mySwapchain, nullptr);
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::PrepareFrame()
{
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::SubmitFrame()
{
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::CreateSwapchain()
{
    VkUtils::SwapChainSupportDetails swapChainSupport =
        VkUtils::QuerySwapChainSupport(myContext.myPhysicalDevice, myContext.mySurface);
    VkUtils::QueueFamilyIndices indices =
        VkUtils::FindQueueFamilies(myContext.myPhysicalDevice, myContext.mySurface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),
                                      indices.presentFamily.value() };

    const VkSurfaceFormatKHR surfaceFormat =
        VkUtils::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    const VkPresentModeKHR presentMode =
        VkUtils::ChooseSwapPresentMode(swapChainSupport.presentMode);
    const VkExtent2D extent = VkUtils::ChooseSwapExtent(swapChainSupport.capabilities,
                                                        myContext.myWidth,
                                                        myContext.myHeight);
    myFrameCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        myFrameCount > swapChainSupport.capabilities.maxImageCount) {
        myFrameCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = myContext.mySurface;
    createInfo.minImageCount = myFrameCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;
    const VkResult result =
        vkCreateSwapchainKHR(myContext.myLogicalDevice, &createInfo, nullptr, &mySwapchain);
    if (result != VK_SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    vkGetSwapchainImagesKHR(myContext.myLogicalDevice, mySwapchain, &myFrameCount, nullptr);
    mySwapchainImages.resize(myFrameCount);
    vkGetSwapchainImagesKHR(myContext.myLogicalDevice,
                            mySwapchain,
                            &myFrameCount,
                            mySwapchainImages.data());
    mySwapchainImageFormat = surfaceFormat.format;
    mySwapchainExtent = extent;
    return RtCode::SUCCESS;
}

Swapchain::RtCode Swapchain::CreateImageViews()
{
    VkImageViewCreateInfo createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mySwapchainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    mySwapchainImageViews.resize(myFrameCount);
    for (unsigned idx = 0; idx < myFrameCount; idx++) {
        createInfo.image = mySwapchainImages[idx];
        const VkResult result = vkCreateImageView(myContext.myLogicalDevice,
                                                  &createInfo,
                                                  nullptr,
                                                  &mySwapchainImageViews[idx]);
        if (result != VK_SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
    }
    return RtCode::SUCCESS;
}

} // namespace Netero::Gfx
