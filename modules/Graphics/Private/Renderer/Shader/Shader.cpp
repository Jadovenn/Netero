/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Vulkan/Context/Context.hpp"
#include "Renderer/Shader/Shader.hpp"
#include "Renderer/Renderer.hpp"

#include "Vulkan/Vulkan.hpp"

namespace Netero::Gfx {

std::unique_ptr<Shader> Shader::New(Renderer &aRenderer, const unsigned *someData, size_t aSize, ShaderStage aStage)
{
    auto* rendererImpl = dynamic_cast<RendererImpl*>(&aRenderer);
    if (!rendererImpl) {
        return nullptr;
    }
    auto shader = std::make_unique<ShaderImpl>(rendererImpl->GetContext(), someData, aSize, aStage);
    if (shader->IsValid()) {
        return shader;
    }
    return nullptr;
}

ShaderImpl::ShaderImpl(Context &aContext, const unsigned *someData, size_t aSize, ShaderStage aStage)
    : myContext(aContext), myIsValid(false), myShaderModule(nullptr), myShaderStage(aStage)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = aSize;
    createInfo.pCode = someData;
    const VkResult result = vkCreateShaderModule(myContext.myLogicalDevice, &createInfo, nullptr, &myShaderModule);
    if (result != VK_SUCCESS) {
        myIsValid = false;
    }
}

} // namespace Netero::Gfx
