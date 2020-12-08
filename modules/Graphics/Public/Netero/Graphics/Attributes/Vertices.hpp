/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Netero/Graphics/Attributes/Attribute.hpp>
#include <Netero/Graphics/Vertex.hpp>

namespace Netero::Gfx {

class Vertices: public Attribute {
    public:
    explicit Vertices(const std::vector<Vertex>& someVertices): myVertices(someVertices) {}
    virtual ~Vertices() = default;

    static std::shared_ptr<Vertices> New(const std::vector<Vertex>& someVertices) {
        auto attr = std::make_shared<Vertices>(someVertices);
        return attr;
    }

    std::vector<Vertex>& GetVertices() { return myVertices; }

    private:


    std::vector<Vertex> myVertices;
};

} // namespace Netero::Gfx
