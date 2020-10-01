/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Netero/Graphics/Geometry.hpp>

#include "Renderer/Drawable/Drawable.hpp"
#include "Vulkan/Context/Context.hpp"

namespace Netero::Gfx {

class GeometryImpl final: public Drawable, public Geometry {
    public:
    ~GeometryImpl() override;

    void SetVertices(const Vertices& someVertices) override;
    void SetVerticesWithIndices(const Vertices& someVertices,
                                        const Indices&  someIndices) override;

    private:
    Context* myContext;

    Vertices    myVertices;
    Indices     myIndices;
};

}
