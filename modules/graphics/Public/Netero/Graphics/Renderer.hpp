/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>

namespace Netero::Gfx {

class GfxObject;

class Renderer {
    public:
    virtual ~Renderer() = default;

    virtual void RegisterDrawable(std::shared_ptr<GfxObject> anObject) = 0;
    virtual void UnRegisterDrawable(std::shared_ptr<GfxObject> anObject) = 0;
};

} // namespace Netero::Gfx
