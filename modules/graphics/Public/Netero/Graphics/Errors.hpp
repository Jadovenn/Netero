/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

namespace Netero {

enum class GfxResult {
    SUCCESS = 0,

    // Status Code
    EXIT,
    OUT_OF_DATE_SWAPCHAIN,

    // Errors Code
    GFX_NOT_INITIALIZED,
    DRIVER_CALL_FAILED,
    PHYSICAL_DEVICE_ERROR,
    PHYSICAL_DEVICE_NOT_SUPPORTED,
    MEMORY_ALLOCATION_FAILED,
    DEBUG_MISSING_VALIDATION_LAYERS,
    DEBUG_FAILED_TO_SETUP_CALLBACKS,
};

}
