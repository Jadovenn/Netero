/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

namespace Netero::Gfx {

struct Frame;

class Drawable {
    public:
    virtual ~Drawable() = default;

    virtual void Draw(Frame& aFrame) = 0;

};

}
