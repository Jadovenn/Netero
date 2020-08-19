/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <Renderer/Renderer.hpp>

namespace Netero::Gfx {

RendererImpl::RendererImpl(Context &aContext)
: myContext(aContext)
{
}

void RendererImpl::RegisterDrawable()
{
}

void RendererImpl::UnRegisterDrawable()
{
}

RendererImpl::RtCode RendererImpl::Build()
{
}

RendererImpl::RtCode RendererImpl::Teardown()
{
}

RendererImpl::RtCode RendererImpl::Update()
{
}

}
