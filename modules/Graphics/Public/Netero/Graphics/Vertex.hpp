/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace Netero::Gfx {

struct Vertex {
    glm::vec3 myCoordonite;
    glm::vec3 myColor;
    glm::vec2 myTextureCoordonite;

    bool operator==(const Vertex& anOther) const
    {
        return myCoordonite == anOther.myCoordonite && myColor == anOther.myColor &&
            myTextureCoordonite == anOther.myTextureCoordonite;
    }
};

} // namespace Netero::Gfx

namespace std {
template<>
struct hash<Netero::Gfx::Vertex> {
    size_t operator()(Netero::Gfx::Vertex const& aVertex) const
    {
        return ((hash<glm::vec3>()(aVertex.myCoordonite) ^
                 hash<glm::vec3>()(aVertex.myColor) << 1)) >>
            1 ^
            (hash<glm::vec2>()(aVertex.myTextureCoordonite) << 1);
    }
};

} // namespace std