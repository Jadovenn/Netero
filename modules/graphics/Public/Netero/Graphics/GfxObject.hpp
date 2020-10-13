/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>

namespace Netero::Gfx {

class Shader;

class GfxObject {
    public:
    virtual ~GfxObject() = default;

    virtual void SetShader(std::shared_ptr<Shader>) = 0;
};

} // namespace Netero::Gfx
