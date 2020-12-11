/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <array>
#include <cassert>

#include <Vulkan/Context/Context.hpp>
#include <Vulkan/VkUtils.hpp>

#include <Swapchain/Swapchain.hpp>

namespace Netero::Gfx {

Swapchain::Swapchain(Context& aContext, uint32_t aMaxInFlightFrameCount)
    : myContext(aContext),
      myRenderPass(aContext),
      myCurrentFrame(0),
      myFrameCount(0),
      myMaximumInFlightFrames(aMaxInFlightFrameCount),
      myIsOutOfDate(true),
      mySwapchain(nullptr),
      mySwapchainExtent { 0, 0 },
      mySwapchainImageFormat(VK_FORMAT_UNDEFINED),
      myDepthImage(nullptr),
      myDepthImageMemory(nullptr),
      myDepthImageView(nullptr),
      myCommandPool(nullptr)
{
}

GfxResult Swapchain::Initialize()
{
    assert(myContext.myLogicalDevice != nullptr);  // Logical must be ready at this point
    assert(myContext.myPhysicalDevice != nullptr); // Physical must be ready at this point
    assert(myContext.mySurface != nullptr);        // Surface must be ready at this point
    CreateSwapchain();
    this->myAvailableFrameSemaphore.resize(myMaximumInFlightFrames);
    this->myRenderedFrameSemaphore.resize(myMaximumInFlightFrames);
    this->myInFlyFence.resize(myMaximumInFlightFrames);
    this->myInFlyFrameFence.resize(myFrameCount, nullptr);
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (unsigned idx = 0; idx < myMaximumInFlightFrames; idx++) {
        VkResult result = vkCreateSemaphore(myContext.myLogicalDevice,
                                            &semaphoreInfo,
                                            nullptr,
                                            &myAvailableFrameSemaphore[idx]);
        if (result != VK_SUCCESS) {
            return GfxResult::DRIVER_CALL_FAILED;
        }
        result = vkCreateSemaphore(myContext.myLogicalDevice,
                                   &semaphoreInfo,
                                   nullptr,
                                   &myRenderedFrameSemaphore[idx]);
        if (result != VK_SUCCESS) {
            return GfxResult::DRIVER_CALL_FAILED;
        }
        result = vkCreateFence(myContext.myLogicalDevice, &fenceInfo, nullptr, &myInFlyFence[idx]);
        if (result != VK_SUCCESS) {
            return GfxResult::DRIVER_CALL_FAILED;
        }
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::Teardown()
{
    Release();
    for (int idx = 0; idx < myMaximumInFlightFrames; idx++) {
        vkDestroySemaphore(myContext.myLogicalDevice, myAvailableFrameSemaphore[idx], nullptr);
        vkDestroySemaphore(myContext.myLogicalDevice, myRenderedFrameSemaphore[idx], nullptr);
        vkDestroyFence(myContext.myLogicalDevice, myInFlyFence[idx], nullptr);
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::Build()
{
    CreateImageViews();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffer();
    CreateGraphicsCommandPool();
    CreateCommandBuffers();
    myIsOutOfDate = false;
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::ReBuild()
{
    Release();
    CreateSwapchain();
    CreateImageViews();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffer();
    CreateGraphicsCommandPool();
    CreateCommandBuffers();
    myIsOutOfDate = false;
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::Release()
{
    vkDeviceWaitIdle(myContext.myLogicalDevice);
    vkDestroyImageView(myContext.myLogicalDevice, myDepthImageView, nullptr);
    vkDestroyImage(myContext.myLogicalDevice, myDepthImage, nullptr);
    vkFreeMemory(myContext.myLogicalDevice, myDepthImageMemory, nullptr);
    for (auto* frameBuffer : mySwapchainFrameBuffers) {
        vkDestroyFramebuffer(myContext.myLogicalDevice, frameBuffer, nullptr);
    }
    vkFreeCommandBuffers(myContext.myLogicalDevice,
                         myCommandPool,
                         myCommandBuffers.size(),
                         myCommandBuffers.data());
    vkDestroyCommandPool(myContext.myLogicalDevice, myCommandPool, nullptr);
    myRenderPass.Release();
    for (auto* imageView : mySwapchainImageViews) {
        vkDestroyImageView(myContext.myLogicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(myContext.myLogicalDevice, mySwapchain, nullptr);
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::PrepareFrame(Frame& aFrame)
{
    vkWaitForFences(myContext.myLogicalDevice,
                    1,
                    &myInFlyFence[myCurrentFrame],
                    VK_TRUE,
                    UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(myContext.myLogicalDevice,
                                            mySwapchain,
                                            UINT64_MAX,
                                            myAvailableFrameSemaphore[myCurrentFrame],
                                            nullptr,
                                            &myFrameIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        myIsOutOfDate = true;
        return GfxResult::OUT_OF_DATE_SWAPCHAIN;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        myIsOutOfDate = true;
        return GfxResult::DRIVER_CALL_FAILED;
    }
    aFrame.myFrameIdx = myFrameIndex;

    if (myInFlyFrameFence[myFrameIndex] != nullptr) {
        vkWaitForFences(myContext.myLogicalDevice,
                        1,
                        &myInFlyFrameFence[myFrameIndex],
                        VK_TRUE,
                        UINT64_MAX);
    }
    myInFlyFrameFence[myFrameIndex] = myInFlyFence[myCurrentFrame];
    aFrame.myCommandBuffer = myCommandBuffers[myFrameIndex];
    // if flag -> 0 keep memory, otherwise VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT to return memory to cmd pool
    vkResetCommandBuffer(aFrame.myCommandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(aFrame.myCommandBuffer, &beginInfo) != VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    std::array<VkClearValue, 2> clearValues {};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = myRenderPass;
    renderPassInfo.framebuffer = mySwapchainFrameBuffers[myFrameIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = mySwapchainExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(aFrame.myCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::SubmitFrame(Frame& aFrame)
{
    vkCmdEndRenderPass(aFrame.myCommandBuffer);
    if (vkEndCommandBuffer(aFrame.myCommandBuffer) != VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    VkSemaphore          waitSemaphores[] = { myAvailableFrameSemaphore[myCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo         submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &aFrame.myCommandBuffer;
    VkSemaphore signalSemaphores[] = { myRenderedFrameSemaphore[myCurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    vkResetFences(myContext.myLogicalDevice, 1, &myInFlyFence[myCurrentFrame]);
    VkResult result =
        vkQueueSubmit(myContext.myGraphicsQueue, 1, &submitInfo, myInFlyFence[myCurrentFrame]);
    if (result != VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { mySwapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &myFrameIndex;
    presentInfo.pResults = nullptr;
    result = vkQueuePresentKHR(myContext.myPresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        myIsOutOfDate = true;
        return GfxResult::OUT_OF_DATE_SWAPCHAIN;
    }
    else if (result != VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    myCurrentFrame = (myCurrentFrame + 1) % myMaximumInFlightFrames;
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateSwapchain()
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
        return GfxResult::DRIVER_CALL_FAILED;
    }
    vkGetSwapchainImagesKHR(myContext.myLogicalDevice, mySwapchain, &myFrameCount, nullptr);
    mySwapchainImages.resize(myFrameCount);
    vkGetSwapchainImagesKHR(myContext.myLogicalDevice,
                            mySwapchain,
                            &myFrameCount,
                            mySwapchainImages.data());
    mySwapchainImageFormat = surfaceFormat.format;
    mySwapchainExtent = extent;
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateImageViews()
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
            return GfxResult::DRIVER_CALL_FAILED;
        }
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateRenderPass()
{
    myRenderPass.Reset();

    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = mySwapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    myRenderPass.AddColorAttachment(colorAttachment, colorAttachmentRef);

    VkAttachmentDescription depthAttachment {};
    depthAttachment.format = VkUtils::FindDepthBufferingImageFormat(myContext.myPhysicalDevice);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depthAttachmentRef {};
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    myRenderPass.AddDepthStencilAttachment(depthAttachment, depthAttachmentRef);

    if (myRenderPass.Build() != GfxResult::SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateDepthResources()
{
    VkFormat depthFormat = VkUtils::FindDepthBufferingImageFormat(myContext.myPhysicalDevice);
    assert(depthFormat != VK_FORMAT_UNDEFINED);
    auto [image, imageMemory] = VkUtils::AllocImage(myContext,
                                                    mySwapchainExtent.width,
                                                    mySwapchainExtent.height,
                                                    depthFormat,
                                                    VK_IMAGE_TILING_OPTIMAL,
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    myDepthImage = image;
    myDepthImageMemory = imageMemory;
    if (!myDepthImage) {
        return GfxResult::MEMORY_ALLOCATION_FAILED;
    }
    myDepthImageView = VkUtils::CreateImageView(myContext.myLogicalDevice,
                                                myDepthImage,
                                                depthFormat,
                                                VK_IMAGE_ASPECT_DEPTH_BIT);
    if (!myDepthImageView) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateFrameBuffer()
{
    mySwapchainFrameBuffers.resize(myFrameCount);
    for (size_t i = 0; i < myFrameCount; i++) {
        std::array<VkImageView, 2> attachments = { mySwapchainImageViews[i], myDepthImageView };

        VkFramebufferCreateInfo frameBufferInfo {};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = myRenderPass;
        frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        frameBufferInfo.pAttachments = attachments.data();
        frameBufferInfo.width = mySwapchainExtent.width;
        frameBufferInfo.height = mySwapchainExtent.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(myContext.myLogicalDevice,
                                &frameBufferInfo,
                                nullptr,
                                &mySwapchainFrameBuffers[i]) != VK_SUCCESS) {
            return GfxResult::DRIVER_CALL_FAILED;
        }
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateGraphicsCommandPool()
{
    VkUtils::QueueFamilyIndices queueFamilyIndices =
        VkUtils::FindQueueFamilies(myContext.myPhysicalDevice, myContext.mySurface);

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(myContext.myLogicalDevice, &poolInfo, nullptr, &myCommandPool) !=
        VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    return GfxResult::SUCCESS;
}

GfxResult Swapchain::CreateCommandBuffers()
{
    myCommandBuffers.resize(myFrameCount);
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = myCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(myCommandBuffers.size());

    if (vkAllocateCommandBuffers(myContext.myLogicalDevice, &allocInfo, myCommandBuffers.data()) !=
        VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
