/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <atomic>
#include <vector>

#include <Netero/Graphics/Drawable.hpp>
#include <Netero/Graphics/Attributes/Attribute.hpp>
#include <Netero/Graphics/Vertex.hpp>

namespace Netero::Gfx {

class Renderer;

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

class Geometry: public Drawable {
    public:
    static std::shared_ptr<Geometry> New(Renderer&);

    template<typename T, typename... Args>
    T* addAttribute(Args... args)
    {
        auto attr = T::New(args...);
        if (attr) {
            return reinterpret_cast<T*>(_addAttribute(AttributesTypeID::GetTypeID<T>(),
                                                      std::dynamic_pointer_cast<Attribute>(attr)));
        }
        else {
            return nullptr;
        }
    }

    template<typename T>
    T* getAttribute()
    {
        return reinterpret_cast<T*>(_getAttribute(AttributesTypeID::GetTypeID<T>()));
    }

    private:
    virtual void* _addAttribute(type_id, std::shared_ptr<Attribute>) = 0;
    virtual void* _getAttribute(type_id) = 0;
};

} // namespace Netero::Gfx
