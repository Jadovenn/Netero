/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Vulkan/Vulkan.hpp>

namespace Netero::Gfx {

class Context;

class Swapchain {
    public:
    enum class RtCode { SUCCESS, DRIVER_CALL_ERROR, OUT_OF_DATE_SWAPCHAIN };

    Swapchain(Context& aContext, uint32_t aMaxInFligthFrameCount);

    // Initialization
    RtCode Initialize();
    RtCode Teardown();

    // Runtime related
    RtCode Build();
    RtCode ReBuild();
    RtCode Release();

    VkFormat GetSwapchainImagesFormat() { return mySwapchainImageFormat; }
    unsigned GetCurrentFrameIdx() { return myCurrentFrame; }
    RtCode   PrepareFrame();
    RtCode   SubmitFrame();

    private:
    RtCode CreateSwapchain();
    RtCode CreateImageViews();

    private:
    Context&  myContext;
    int       myCurrentFrame;
    uint32_t  myFrameCount;
    const int myMaximumInFligthFrames;

    // Synchronization related
    std::vector<VkSemaphore> myAvailableFrameSemaphore;
    std::vector<VkSemaphore> myRenderedFrameSemaphore;
    std::vector<VkFence>     myInFlyFrameFence;
    std::vector<VkFence>     myRenderedFrameFence;

    // Swapchain related
    VkSwapchainKHR             mySwapchain;
    VkExtent2D                 mySwapchainExtent;
    std::vector<VkImage>       mySwapchainImages;
    VkFormat                   mySwapchainImageFormat;
    std::vector<VkImageView>   mySwapchainImageViews;
    std::vector<VkFramebuffer> mySwapchainFrameBuffers;
};

} // namespace Netero::Gfx
