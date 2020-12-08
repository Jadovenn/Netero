/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Graphics/Attributes/Shader.hpp>
#include <Netero/Graphics/Renderer.hpp>

#include "Renderer/Shader/Default/GeometryFragment.h"
#include "Renderer/Shader/Default/GeometryVertex.h"
#include "Renderer/Shader/DefaultShaders.hpp"

namespace Netero::Gfx {

bool CheckGeometryShaders(Renderer *aRenderer, Geometry &aGeometry)
{
    if (!aGeometry.getAttribute<VertexShader>()) {
        aGeometry.addAttribute<VertexShader>(aRenderer,
                                             GeometryVertexData,
                                             GeometryVertexWordCount * sizeof(unsigned));
    }
    if (!aGeometry.getAttribute<FragmentShader>()) {
        aGeometry.addAttribute<FragmentShader>(aRenderer,
                                               GeometryFragmentData,
                                               GeometryVertexWordCount * sizeof(unsigned));
    }
    return true;
}

} // namespace Netero::Gfx
