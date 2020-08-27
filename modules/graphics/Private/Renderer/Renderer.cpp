/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <Vulkan/Buffer/Buffer.hpp>

#include <Renderer/Renderer.hpp>

namespace Netero::Gfx {

RendererImpl::RendererImpl(Context &aContext): myContext(aContext)
{
}

void RendererImpl::RegisterDrawable()
{
}

void RendererImpl::UnRegisterDrawable()
{
}

GfxResult RendererImpl::Initialize()
{
    Buffer<int> buffer(myContext,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    buffer.Reserve(64);
    int data[5] = { 1, 2, 3, 4, 5 };
    buffer.Write(0, data, 5);
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::Teardown()
{
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::Build()
{
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::Release()
{
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::ReBuild()
{
    Release();
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::Update()
{
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
