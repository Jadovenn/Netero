/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Vulkan/RenderPass/RenderPass.hpp>

namespace Netero::Gfx {

void RenderPass::AddColorAttachment(VkAttachmentDescription &anAttachment,
                                    VkAttachmentReference &  aReference)
{
    aReference.attachment = myAttachmentDescriptions.size();
    myAttachmentDescriptions.push_back(anAttachment);
    myColorAttachmentReferences.push_back(aReference);
}

void RenderPass::AddDepthStencilAttachment(VkAttachmentDescription &anAttachment,
                                           VkAttachmentReference &  aReference)
{
    aReference.attachment = myAttachmentDescriptions.size();
    myAttachmentDescriptions.push_back(anAttachment);
    myDepthStencilAttachmentReference = aReference;
}

void RenderPass::Reset()
{
    myDepthStencilAttachmentReference = {};
    myColorAttachmentReferences.clear();
    myAttachmentDescriptions.clear();
}

GfxResult RenderPass::Build()
{
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = myColorAttachmentReferences.size();
    subpass.pColorAttachments = myColorAttachmentReferences.data();
    subpass.pDepthStencilAttachment = &myDepthStencilAttachmentReference;
    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(myAttachmentDescriptions.size());
    renderPassInfo.pAttachments = myAttachmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    const VkResult result =
        vkCreateRenderPass(myContext.myLogicalDevice, &renderPassInfo, nullptr, &myRenderPass);
    if (result != VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }
    return GfxResult::SUCCESS;
}

GfxResult RenderPass::Release()
{
    vkDestroyRenderPass(myContext.myLogicalDevice, myRenderPass, nullptr);
    return GfxResult::SUCCESS;
}

GfxResult RenderPass::Rebuild()
{
    auto result = Release();
    if (result != GfxResult::SUCCESS) {
        return result;
    }
    result = Build();
    if (result != GfxResult::SUCCESS) {
        return result;
    }
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
