/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Renderer.hpp"

#include "Renderer/Drawable/Drawable.hpp"
#include "Vulkan/Buffer/Buffer.hpp"

namespace Netero::Gfx {

RendererImpl::RendererImpl(Context &aContext)
    : myContext(aContext), mySwapchain(myContext, 2), myCamera(nullptr)
{
}

GfxResult RendererImpl::RegisterDrawable(std::shared_ptr<PublicDrawable> aDrawable)
{
    auto result = myDrawables.insert(std::dynamic_pointer_cast<Drawable>(aDrawable));
    if (result.second) {
        return (*result.first)->Initialize();
    }
    return GfxResult::FAILED;
}

GfxResult RendererImpl::UnRegisterDrawable(std::shared_ptr<PublicDrawable> aDrawable)
{
    if (auto drawable = std::dynamic_pointer_cast<Drawable>(aDrawable)) {
        drawable->Teardown();
        myDrawables.erase(drawable);
        return GfxResult::SUCCESS;
    }
    return GfxResult::FAILED;
}

void RendererImpl::AttachCamera(Camera *aCamera)
{
    myCamera = aCamera;
}

void RendererImpl::DetachCamera()
{
    myCamera = nullptr;
}

bool RendererImpl::IsSwapchainOutOfDate() const
{
    return mySwapchain.IsOutOfDate();
}

GfxResult RendererImpl::Initialize()
{
    Buffer<int> buffer(myContext,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    buffer.Reserve(64);
    int data[5] = { 1, 2, 3, 4, 5 };
    buffer.Write(0, data, 5);
    auto result = mySwapchain.Initialize();
    if (result != GfxResult::SUCCESS) {
        return result;
    }
    return result;
}

GfxResult RendererImpl::Teardown()
{
    auto result = mySwapchain.Teardown();
    if (result != GfxResult::SUCCESS) {
        return result;
    }
    return result;
}

GfxResult RendererImpl::Build()
{
    mySwapchain.Build();
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::Release()
{
    mySwapchain.Release();
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::ReBuild()
{
    mySwapchain.ReBuild();
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::Update(Frame &)
{
    // TODO: Update Uniform Buffer Object
    // TODO: Record draw calls from registered drawable
    return GfxResult::SUCCESS;
}

GfxResult RendererImpl::PresentFrame()
{
    Frame frame;
    auto  result = mySwapchain.PrepareFrame(frame);
    if (result != GfxResult::SUCCESS) {
        return GfxResult::SUCCESS;
    }
    Update(frame);
    result = mySwapchain.SubmitFrame(frame);
    if (result != GfxResult::SUCCESS) {
        return result;
    }
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
