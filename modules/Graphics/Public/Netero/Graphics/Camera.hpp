/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>

#include <Netero/Graphics/Geometry.hpp>

namespace Netero::Gfx {

enum class CameraMode {
    LOOK_AT,
    FIRST_PERSON,
};

class Camera {
    public:
    Camera(CameraMode aCameraMode): myCameraMode(aCameraMode) {}

    Position myPosition;
    Position myLookAt;

    private:
    CameraMode myCameraMode;
};

} // namespace Netero::Gfx
