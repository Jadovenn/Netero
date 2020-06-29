/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <atomic>

namespace netero::graphics {

/**
 * @brief An axis container with value, rotation and scale ratio attributes.
 */
class Axis {
    std::atomic<float> _value = 0;          /**< Value of the axis. */
    std::atomic<float> _rotation = 0;       /**< Rotation to be applied to the axis in radian. */
    std::atomic<float> _scale = 1;          /**< Scale ratio to be applied to the axis. */
    std::atomic<bool>  _has_changed = true; /**< Not used please ignore. */
    public:
    /**
     * Test if the axis had been altered.
     */
    [[nodiscard]] bool isAltered()
    {
        if (_has_changed) {
            _has_changed = false;
            return true;
        }
        return false;
    }

    /**
     * Return the value of the axis.
     */
    operator float() const { return this->_value; }

    /**
     * Assign the axis value.
     */
    Axis& operator=(float value)
    {
        this->_has_changed = true;
        this->_value = value;
        return *this;
    };

    /**
     * Apply a rotation to the axis.
     */
    void rotate(float value)
    {
        this->_has_changed = true;
        this->_rotation = value;
    }

    /**
     * Get actual rotation of the axis.
     */
    [[nodiscard]] float getRotation() const { return this->_rotation; }

    /**
     * Apply a scaling ratio to the axis.
     */
    void scale(float value)
    {
        this->_has_changed = true;
        this->_scale = value;
    }

    /**
     * Return actual scale ratio
     */
    [[nodiscard]] float getScaling() const { return this->_scale; }
};
} // namespace netero::graphics
