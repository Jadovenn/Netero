
#pragma once

#include <Vulkan/Context/Context.hpp>
#include <Vulkan/VkUtils.hpp>
#include <Vulkan/Vulkan.hpp>

#include <Netero/Graphics/Errors.hpp>
#include <netero/logger.hpp>

namespace Netero::Gfx {

template<class T>
class Buffer {
    public:
    Buffer(Context&              myContext,
           VkBufferUsageFlags    someBufferFlags,
           VkMemoryPropertyFlags someMemoryFlags)
        : myContext(myContext),
          myBufferFlags(someBufferFlags),
          myMemoryFlags(someMemoryFlags),
          myItemCount(0),
          mySizeInByte(0),
          myBuffer(nullptr),
          myBufferMemory(nullptr)
    {
    }

    ~Buffer() { Free(); }

    GfxResult Reserve(size_t anItemCount)
    {
        if (myBuffer) {
            Free();
        }
        myItemCount = anItemCount;
        mySizeInByte = sizeof(T) * myItemCount;
        return InternalAlloc();
    }

    void Free()
    {
        if (myBuffer) {
            vkDestroyBuffer(myContext.myLogicalDevice, myBuffer, nullptr);
            vkFreeMemory(myContext.myLogicalDevice, myBufferMemory, nullptr);
            myBuffer = nullptr;
            myBufferMemory = nullptr;
            myItemCount = -1;
            mySizeInByte = -1;
        }
    }

    void Write(size_t anIndex, T* someData, size_t anItemCount)
    {
        void* data;
        vkMapMemory(myContext.myLogicalDevice,
                    myBufferMemory,
                    anIndex,
                    sizeof(T) * anItemCount,
                    0,
                    &data);
        memcpy(data, someData, sizeof(T) * anItemCount);
        vkUnmapMemory(myContext.myLogicalDevice, myBufferMemory);
    }

    void operator>>(Buffer& aBuffer)
    {
        aBuffer.Reserve(myItemCount);
        VkCommandBuffer cmdBuffer =
            VkUtils::BeginCommandBufferRecording(myContext.myLogicalDevice,
                                                 myContext.myTransferCommandPool,
                                                 VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VkBufferCopy copyRegion {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = mySizeInByte;
        vkCmdCopyBuffer(cmdBuffer, myBuffer, aBuffer.myBuffer, 1, &copyRegion);
        VkUtils::EndCommandBuffer(cmdBuffer);

        VkUtils::FlushCommandBuffer(myContext.myLogicalDevice,
                                    myContext.myTransferQueue,
                                    myContext.myTransferCommandPool,
                                    cmdBuffer);
    }

    size_t   Size() { return myItemCount; }
    size_t   SizeInByte() { return mySizeInByte; }
    VkBuffer Data() { return myBuffer; }

    private:
    GfxResult InternalAlloc()
    {
        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = mySizeInByte;
        bufferInfo.usage = myBufferFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(myContext.myLogicalDevice, &bufferInfo, nullptr, &myBuffer) !=
            VK_SUCCESS) {
            return GfxResult::MEMORY_ALLOCATION_FAILED;
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(myContext.myLogicalDevice, myBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VkUtils::FindMemoryType(myContext.myPhysicalDevice,
                                                            memRequirements.memoryTypeBits,
                                                            myMemoryFlags);

        if (vkAllocateMemory(myContext.myLogicalDevice, &allocInfo, nullptr, &myBufferMemory) !=
            VK_SUCCESS) {
            LOG_ERROR << "Failed to allocate buffer memory!" << std::endl;
            return GfxResult::MEMORY_ALLOCATION_FAILED;
        }
        vkBindBufferMemory(myContext.myLogicalDevice, myBuffer, myBufferMemory, 0);
        return GfxResult::SUCCESS;
    }

    Context&              myContext;
    VkBufferUsageFlags    myBufferFlags;
    VkMemoryPropertyFlags myMemoryFlags;

    size_t         myItemCount;
    size_t         mySizeInByte;
    VkBuffer       myBuffer;
    VkDeviceMemory myBufferMemory;
};

} // namespace Netero::Gfx
