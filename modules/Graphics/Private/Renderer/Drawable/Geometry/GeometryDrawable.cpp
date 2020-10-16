/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Drawable/Geometry/Geometry.hpp"

namespace Netero::Gfx {

GfxResult GeometryImpl::Initialize()
{
    { // Transfer Vertices to GPU
        Buffer<Vertex> verticesTransferBuffer(myContext,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        verticesTransferBuffer.Reserve(myVertices.size());
        verticesTransferBuffer.Write(0, myVertices.data(), myVertices.size());
        verticesTransferBuffer >> myVerticesBuffer;
    }
    { // Transfer Indices to GPU
        Buffer<uint32_t> indicesTransferBuffer(myContext,
                                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        indicesTransferBuffer.Reserve(myIndices.size());
        indicesTransferBuffer.Write(0, myIndices.data(), myIndices.size());
        indicesTransferBuffer >> myIndicesBuffer;
    }
    return GfxResult::SUCCESS;
}

GfxResult GeometryImpl::Teardown()
{
    myVerticesBuffer.Free();
    myIndicesBuffer.Free();
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
