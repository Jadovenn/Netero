/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Drawable/Geometry.hpp"

#include <memory>

namespace Netero::Gfx {

GeometryImpl::~GeometryImpl()
{
    Release();
    Teardown();
}

std::shared_ptr<Geometry> Geometry::New()
{
    return std::make_shared<GeometryImpl>();
}

void GeometryImpl::SetVertices(const Vertices &someVertices)
{
    myVertices = someVertices;
    for (unsigned index = 0; index < myVertices.size(); ++index) {
        myIndices.push_back(index);
    }
}

void GeometryImpl::SetVerticesWithIndices(const Vertices &someVertices, const Indices &someIndices)
{
    myVertices = someVertices;
    myIndices = someIndices;
}

} // namespace Netero::Gfx
