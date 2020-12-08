/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <map>
#include <utility>
#include <vector>

#include <Netero/Graphics/Drawable.hpp>
#include <Netero/Graphics/Attributes/Attribute.hpp>
#include <Netero/Graphics/Geometry.hpp>

#include "Vulkan/Buffer/Buffer.hpp"
#include "Vulkan/Context/Context.hpp"

namespace Netero::Gfx {

class GeometryImpl final: public Geometry {
    public:
    explicit GeometryImpl(Context& aContext);
    ~GeometryImpl() override;

    GfxResult Initialize() final;
    GfxResult Teardown() final;

    private:
    void* _addAttribute(type_id, std::shared_ptr<Attribute>) final;
    void* _getAttribute(type_id) final;

    std::map<Netero::type_id, std::shared_ptr<Netero::Gfx::Attribute>> myAttributes;

    private:
    Context& myContext;

    //Vertices myVertices;
    //Indices  myIndices;

    Buffer<Vertex>   myVerticesBuffer;
    Buffer<uint32_t> myIndicesBuffer;
};

} // namespace Netero::Gfx
