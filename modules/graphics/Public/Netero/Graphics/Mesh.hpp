/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Graphics/Geometry.hpp>

namespace Netero::Gfx {

class Texture;

class Mesh {
    public:
    virtual ~Mesh() = default;

    using GeometryVector = std::vector<Vertex>;
    using IndexVector = std::vector<uint32_t>;

    virtual void AddGeometry(GeometryVector&& aGeometryIn, IndexVector&& someIndicesIn) = 0;
    virtual void SetTexture(Texture*);
};

} // namespace Netero::Gfx
