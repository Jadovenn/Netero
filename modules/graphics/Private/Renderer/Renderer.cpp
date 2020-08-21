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

RendererImpl::RtCode RendererImpl::Initialize()
{
    return RtCode::SUCCESS;
}

RendererImpl::RtCode RendererImpl::Teardown()
{
    return RtCode::SUCCESS;
}

RendererImpl::RtCode RendererImpl::Build()
{
    return RtCode::SUCCESS;
}

RendererImpl::RtCode RendererImpl::Release()
{
    return RtCode::SUCCESS;
}

RendererImpl::RtCode RendererImpl::ReBuild()
{
    Release();
    return RtCode::SUCCESS;
}

RendererImpl::RtCode RendererImpl::Update()
{
    return RtCode::SUCCESS;
}

} // namespace Netero::Gfx
