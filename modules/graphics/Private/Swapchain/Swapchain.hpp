/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Vulkan/RenderPass/RenderPass.hpp>
#include <Vulkan/Vulkan.hpp>

namespace Netero::Gfx {

class Context;

struct Frame {
    uint32_t        myFrameIdx;
    VkCommandBuffer myCommandBuffer;
};

class Swapchain {
    public:
    enum class RtCode {
        SUCCESS,
        DRIVER_CALL_ERROR,
        OUT_OF_DATE_SWAPCHAIN,
        MEMORY_ALLOCATION_ERROR
    };

    Swapchain(Context& aContext, uint32_t aMaxInFlightFrameCount);

#pragma region Initialization Methods

    RtCode Initialize();
    RtCode Teardown();

#pragma region Runtime Methods

    RtCode Build();
    RtCode ReBuild();
    RtCode Release();

#pragma region Utility Methods

    VkFormat GetSwapchainImagesFormat() { return mySwapchainImageFormat; }
    unsigned GetCurrentFrameIdx() { return myCurrentFrame; }
    uint32_t GetFrameCount() { return myFrameCount; }
    RtCode   PrepareFrame(Frame& aFrame);
    RtCode   SubmitFrame(Frame& aFrame);

#pragma region Private Utility Methods

    private:
    RtCode CreateSwapchain();
    RtCode CreateImageViews();
    RtCode CreateRenderPass();
    RtCode CreateDepthResources();
    RtCode CreateFrameBuffer();
    RtCode CreateGraphicsCommandPool();
    RtCode CreateCommandBuffers();

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
