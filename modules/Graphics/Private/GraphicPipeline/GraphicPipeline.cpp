/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "GraphicPipeline/GraphicPipeline.hpp"

#include "Swapchain/Swapchain.hpp"

namespace Netero::Gfx {

GraphicPipeline::GraphicPipeline(Context &aContext): myContext(aContext)
{
}

GfxResult GraphicPipeline::Initialize()
{
    return GfxResult::SUCCESS;
}

GfxResult GraphicPipeline::Teardown()
{
    return GfxResult::SUCCESS;
}

GfxResult GraphicPipeline::Build()
{
    return GfxResult::SUCCESS;
}

GfxResult GraphicPipeline::ReBuild()
{
    return GfxResult::SUCCESS;
}

GfxResult GraphicPipeline::Update(Frame &)
{
    // TODO: Apply transform
    return GfxResult::SUCCESS;
}

GfxResult GraphicPipeline::CommitDrawCalls(Frame &)
{
    // TODO: Commit draw call to the command buffer
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
