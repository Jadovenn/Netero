/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Vulkan/Context/Context.hpp>
#include <Vulkan/VkUtils.hpp>

#include <Netero/Graphics/Errors.hpp>

namespace Netero::Gfx {

class RenderPass {
    public:
    explicit RenderPass(Context& aContext)
        : myContext(aContext), myDepthStencilAttachmentReference {}, myRenderPass(nullptr)
    {
    }

    operator VkRenderPass() { return myRenderPass; }

    void AddColorAttachment(VkAttachmentDescription& anAttachment,
                            VkAttachmentReference&   aReference);
    void AddDepthStencilAttachment(VkAttachmentDescription& anAttachment,
                                   VkAttachmentReference&   aReference);

    void Reset();

    GfxResult Build();
    GfxResult Release();
    GfxResult Rebuild();

    private:
    Context& myContext;

    VkAttachmentReference                myDepthStencilAttachmentReference;
    std::vector<VkAttachmentReference>   myColorAttachmentReferences;
    std::vector<VkAttachmentDescription> myAttachmentDescriptions;

    VkRenderPass myRenderPass;
};

} // namespace Netero::Gfx
