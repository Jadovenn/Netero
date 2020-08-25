/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <atomic>

namespace Netero::Gfx {

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

} // namespace Netero::Gfx
