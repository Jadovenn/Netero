/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Drawable/Geometry/Geometry.hpp"

#include <memory>

#include "Renderer/Renderer.hpp"
#include "Renderer/Shader/Shader.hpp"

namespace Netero::Gfx {

GeometryImpl::GeometryImpl(Context &aContext)
    : myContext(aContext),
      myVerticesBuffer(myContext,
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
      myIndicesBuffer(myContext,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
}

GeometryImpl::~GeometryImpl()
{
    Release();
    Teardown();
}

void *GeometryImpl::_addAttribute(type_id aTypeID, std::shared_ptr<Attribute> anAttribute)
{
    if (myAttributes.find(aTypeID) == myAttributes.end()) {
        myAttributes.insert({ aTypeID, anAttribute });
        return anAttribute.get();
    }
    else {
        return nullptr;
    }
}

void *GeometryImpl::_getAttribute(type_id aTypeID)
{
    auto it = myAttributes.find(aTypeID);
    if (it != myAttributes.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::shared_ptr<Geometry> Geometry::New(Renderer &aRenderer)
{
    if (auto *renderer = dynamic_cast<RendererImpl *>(&aRenderer)) {
        return std::make_unique<GeometryImpl>(renderer->GetContext());
    }
    return nullptr;
}

} // namespace Netero::Gfx
