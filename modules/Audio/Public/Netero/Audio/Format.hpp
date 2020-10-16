/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

namespace Netero::Audio {

struct Format {
    unsigned           myFramesCount = 0;
    unsigned           myBytesPerFrame = 0;
    unsigned           myBytesPerSample = 0;
    unsigned           myChannels = 0;
    unsigned           mySamplingFrequency = 0;
    std::vector<float> mySupportedSamplingRate = {};
};

} // namespace Netero::Audio
