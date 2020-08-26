/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

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
