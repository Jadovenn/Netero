/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <array>
#include <utility>
#include <vector>

#include <Netero/Graphics/Geometry.hpp>

#include "Renderer/Drawable/Drawable.hpp"
#include "Vulkan/Buffer/Buffer.hpp"
#include "Vulkan/Context/Context.hpp"

namespace Netero::Gfx {

class GeometryImpl final: public Drawable, public Geometry {
    public:
    explicit GeometryImpl(Context& aContext);
    ~GeometryImpl() override;

    void SetVertices(const Vertices& someVertices) override;
    void SetVerticesWithIndices(const Vertices& someVertices, const Indices& someIndices) override;

    void SetShader(std::shared_ptr<Shader>) override;

    GfxResult Initialize() final;
    GfxResult Teardown() final;

    private:
    Context&                               myContext;
    std::array<std::shared_ptr<Shader>, 4> myShaders;

    Vertices myVertices;
    Indices  myIndices;

    Buffer<Vertex>   myVerticesBuffer;
    Buffer<uint32_t> myIndicesBuffer;
};

} // namespace Netero::Gfx
