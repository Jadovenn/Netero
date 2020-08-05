/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

namespace netero::audio {

struct Format {
    unsigned           framesCount = 0;
    unsigned           bytesPerFrame = 0;
    unsigned           bytesPerSample = 0;
    unsigned           channels = 0;
    unsigned           samplingFrequency = 0;
    std::vector<float> supportedSamplingRate = {};
};

} // namespace netero::audio
