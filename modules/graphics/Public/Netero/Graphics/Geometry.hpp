/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <atomic>
#include <unordered_map>
#include <vector>

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

using Vertices = std::vector<Vertex>;
using Indices = std::vector<unsigned>;

/**
 * @brief An axis container with value, rotation and scale ratio attributes.
 */
class Axis {
    public:
    /**
     * Test if the axis had been altered.
     */
    [[nodiscard]] bool IsAltered()
    {
        if (myHasChanged) {
            myHasChanged = false;
            return true;
        }
        return false;
    }

    /**
     * Return the value of the axis.
     */
    operator float() const { return myValue; }

    /**
     * Assign the axis value.
     */
    Axis& operator=(float value)
    {
        myHasChanged = true;
        myValue = value;
        return *this;
    };

    /**
     * Apply a rotation to the axis.
     */
    void Rotate(float value)
    {
        myHasChanged = true;
        myRotation = value;
    }

    /**
     * Get actual rotation of the axis.
     */
    [[nodiscard]] float GetRotation() const { return myRotation; }

    /**
     * Apply a scaling ratio to the axis.
     */
    void Scale(float value)
    {
        myHasChanged = true;
        myScale = value;
    }

    /**
     * Return actual scale ratio
     */
    [[nodiscard]] float GetScaling() const { return myScale; }

    private:
    std::atomic<float> myValue = 0;         /**< Value of the axis. */
    std::atomic<float> myRotation = 0;      /**< Rotation to be applied to the axis in radian. */
    std::atomic<float> myScale = 1;         /**< Scale ratio to be applied to the axis. */
    std::atomic<bool>  myHasChanged = true; /**< Not used please ignore. */
};

class Geometry {
    public:
    static std::shared_ptr<Geometry> New();

    virtual void SetVertices(const Vertices& someVertices) = 0;
    virtual void SetVerticesWithIndices(const Vertices& someVertices,
                                        const Indices&  someIndices) = 0;
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
