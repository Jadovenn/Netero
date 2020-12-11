/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Vulkan/RenderPass/RenderPass.hpp>
#include <Vulkan/Vulkan.hpp>

#include <Netero/Graphics/Errors.hpp>

namespace Netero::Gfx {

class Context;

struct Frame {
    uint32_t        myFrameIdx;
    VkCommandBuffer myCommandBuffer;
};

class Swapchain {
    public:
    Swapchain(Context& aContext, uint32_t aMaxInFlightFrameCount);

#pragma region Initialization Methods

    GfxResult Initialize();
    GfxResult Teardown();

#pragma region Runtime Methods

    GfxResult Build();
    GfxResult ReBuild();
    GfxResult Release();

#pragma region Utility Methods

    VkFormat  GetSwapchainImagesFormat() { return mySwapchainImageFormat; }
    unsigned  GetCurrentFrameIdx() { return myCurrentFrame; }
    uint32_t  GetFrameCount() { return myFrameCount; }
    GfxResult PrepareFrame(Frame& aFrame);
    GfxResult SubmitFrame(Frame& aFrame);
    bool      IsOutOfDate() const { return myIsOutOfDate; }

#pragma region Private Utility Methods

    private:
    GfxResult CreateSwapchain();
    GfxResult CreateImageViews();
    GfxResult CreateRenderPass();
    GfxResult CreateDepthResources();
    GfxResult CreateFrameBuffer();
    GfxResult CreateGraphicsCommandPool();
    GfxResult CreateCommandBuffers();

#pragma region Private Attributes

    private:
    Context&   myContext;
    RenderPass myRenderPass;
    uint32_t   myCurrentFrame;
    uint32_t   myFrameCount;
    uint32_t   myFrameIndex;
    const int  myMaximumInFlightFrames;

#pragma region - Synchronization Attributes

    std::vector<VkSemaphore> myAvailableFrameSemaphore;
    std::vector<VkSemaphore> myRenderedFrameSemaphore;
    std::vector<VkFence>     myInFlyFence;
    std::vector<VkFence>     myInFlyFrameFence;

#pragma region - Swapchain Attributes

    bool                       myIsOutOfDate;
    VkSwapchainKHR             mySwapchain;
    VkExtent2D                 mySwapchainExtent;
    std::vector<VkImage>       mySwapchainImages;
    VkFormat                   mySwapchainImageFormat;
    std::vector<VkImageView>   mySwapchainImageViews;
    std::vector<VkFramebuffer> mySwapchainFrameBuffers;

#pragma region - Depth Attributes

    VkImage        myDepthImage;
    VkDeviceMemory myDepthImageMemory;
    VkImageView    myDepthImageView;

#pragma region - CommandBuffers Attributes

    VkCommandPool                myCommandPool;
    std::vector<VkCommandBuffer> myCommandBuffers;
};

} // namespace Netero::Gfx
