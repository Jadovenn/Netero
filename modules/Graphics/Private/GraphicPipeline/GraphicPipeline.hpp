/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include "Renderer/Drawable/Drawable.hpp"

namespace Netero::Gfx {

class Frame;
class Context;

class GraphicPipeline: public Drawable {
    public:
    GraphicPipeline(Context& aContext);

    GfxResult Initialize() final;
    GfxResult Teardown() final;
    GfxResult Build() final;
    GfxResult ReBuild() final;
    GfxResult Update(Frame&) final;

    GfxResult CommitDrawCalls(Frame&);

    private:
    Context&  myContext;
};

} // namespace Netero::Gfx
