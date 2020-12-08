/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Netero/Graphics/Attributes/Attribute.hpp>
#include <Netero/Graphics/Vertex.hpp>

namespace Netero::Gfx {

class Indices: public Attribute {
    public:
    explicit Indices(const std::vector<uint32_t>& someIndices): myIndices(someIndices) {}
    virtual ~Indices() = default;

    static std::shared_ptr<Indices> New(const std::vector<uint32_t>& someIndices)
    {
        auto attr = std::make_shared<Indices>(someIndices);
        return attr;
    }

    std::vector<uint32_t>& GetIndices() { return myIndices; }

    private:

    std::vector<uint32_t> myIndices;
};

} // namespace Netero::Gfx
