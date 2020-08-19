/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Window/Window_GLFW.hpp>

namespace Netero::Gfx {

WindowGLFW::WindowGLFW(uint32_t           aWidth,
                       uint32_t           anHeight,
                       WindowMode         aMode,
                       const std::string& aTitle)

    : myWidth(aWidth), myHeight(anHeight), myMode(aMode), myTitle(aTitle)
{
}

void WindowGLFW::SetPosition(uint32_t anXAxis, uint32_t anYAxis)
{
}

} // namespace Netero::Gfx
