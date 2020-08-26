/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstdint>

namespace Netero::Gfx {

enum class TextureSamplingMode {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    MIRRORED_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

class Texture {
    public:
    ~Texture() = default;

    virtual void
    Load(void *someData, uint32_t aWidth, uint32_t aHeight, TextureSamplingMode aSamplingMode) = 0;
};

} // namespace Netero::Gfx
