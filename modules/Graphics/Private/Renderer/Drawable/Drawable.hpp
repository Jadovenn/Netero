/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Graphics/Errors.hpp>
#include <Netero/Graphics/PublicDrawable.hpp>

namespace Netero::Gfx {

class Frame;

class Drawable {
    public:
    virtual ~Drawable() = default;

    virtual GfxResult Initialize() { return GfxResult::SUCCESS; }

    virtual GfxResult Teardown() { return GfxResult::SUCCESS; }

    virtual GfxResult Build() { return GfxResult::SUCCESS; }

    virtual GfxResult Release() { return GfxResult::SUCCESS; }

    virtual GfxResult ReBuild() { return GfxResult::SUCCESS; }

    virtual GfxResult Update(Frame&) { return GfxResult::SUCCESS; }
};

} // namespace Netero::Gfx
