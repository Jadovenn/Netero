/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Logger.hpp>

#include <Netero/Graphics/Attributes/Indices.hpp>
#include <Netero/Graphics/Attributes/Vertices.hpp>

#include "Renderer/Drawable/Geometry/Geometry.hpp"

namespace Netero::Gfx {

GfxResult GeometryImpl::Initialize()
{
    GfxResult result = GfxResult::SUCCESS;
    if (auto* attr = getAttribute<Vertices>()) { // Transfer Vertices to GPU
        auto&          vertices = attr->GetVertices();
        Buffer<Vertex> verticesTransferBuffer(myContext,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        verticesTransferBuffer.Reserve(vertices.size());
        verticesTransferBuffer.Write(0, vertices.data(), vertices.size());
        verticesTransferBuffer >> myVerticesBuffer;
    }
    else {
        LOG_ERROR << "Vertices attribute not present on Geometry." << std::endl;
        result = GfxResult::MISSING_ATTRIBUTES;
    }
    if (auto* attr = getAttribute<Indices>()) { // Transfer Indices to GPU
        auto&            indices = attr->GetIndices();
        Buffer<uint32_t> indicesTransferBuffer(myContext,
                                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        indicesTransferBuffer.Reserve(indices.size());
        indicesTransferBuffer.Write(0, indices.data(), indices.size());
        indicesTransferBuffer >> myIndicesBuffer;
    }
    else {
        LOG_ERROR << "Indices attribute not present on Geometry." << std::endl;
        result = GfxResult::MISSING_ATTRIBUTES;
    }
    return result;
}

GfxResult GeometryImpl::Teardown()
{
    myVerticesBuffer.Free();
    myIndicesBuffer.Free();
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
