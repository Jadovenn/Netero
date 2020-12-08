/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Shader/Shader.hpp"

#include "Renderer/Renderer.hpp"
#include "Vulkan/Context/Context.hpp"
#include "Vulkan/Vulkan.hpp"

namespace Netero::Gfx {

std::shared_ptr<Shader>
VertexShader::New(Renderer *aRenderer, const unsigned *someData, size_t aSize)
{
    auto *rendererImpl = dynamic_cast<RendererImpl *>(aRenderer);
    if (!rendererImpl) {
        return nullptr;
    }
    auto shader = std::make_unique<ShaderImpl>(rendererImpl->GetContext(),
                                               someData,
                                               aSize,
                                               ShaderStage::VERTEX);
    if (shader->IsValid()) {
        return shader;
    }
    return nullptr;
}

std::shared_ptr<Shader>
TessellationShader::New(Renderer *aRenderer, const unsigned *someData, size_t aSize)
{
    auto *rendererImpl = dynamic_cast<RendererImpl *>(aRenderer);
    if (!rendererImpl) {
        return nullptr;
    }
    auto shader = std::make_unique<ShaderImpl>(rendererImpl->GetContext(),
                                               someData,
                                               aSize,
                                               ShaderStage::TESSELLATION);
    if (shader->IsValid()) {
        return shader;
    }
    return nullptr;
}

std::shared_ptr<Shader>
GeometryShader::New(Renderer *aRenderer, const unsigned *someData, size_t aSize)
{
    auto *rendererImpl = dynamic_cast<RendererImpl *>(aRenderer);
    if (!rendererImpl) {
        return nullptr;
    }
    auto shader = std::make_unique<ShaderImpl>(rendererImpl->GetContext(),
                                               someData,
                                               aSize,
                                               ShaderStage::GEOMETRY);
    if (shader->IsValid()) {
        return shader;
    }
    return nullptr;
}

std::shared_ptr<Shader>
FragmentShader::New(Renderer *aRenderer, const unsigned *someData, size_t aSize)
{
    auto *rendererImpl = dynamic_cast<RendererImpl *>(aRenderer);
    if (!rendererImpl) {
        return nullptr;
    }
    auto shader = std::make_unique<ShaderImpl>(rendererImpl->GetContext(),
                                               someData,
                                               aSize,
                                               ShaderStage::FRAGMENT);
    if (shader->IsValid()) {
        return shader;
    }
    return nullptr;
}

ShaderImpl::ShaderImpl(Context &       aContext,
                       const unsigned *someData,
                       size_t          aSize,
                       ShaderStage     aStage)
    : myContext(aContext), myIsValid(false), myShaderModule(nullptr), myShaderStage(aStage)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = aSize;
    createInfo.pCode = someData;
    const VkResult result =
        vkCreateShaderModule(myContext.myLogicalDevice, &createInfo, nullptr, &myShaderModule);
    if (result != VK_SUCCESS) {
        myIsValid = false;
    }
}

} // namespace Netero::Gfx
