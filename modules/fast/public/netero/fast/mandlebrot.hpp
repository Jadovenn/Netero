/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

namespace netero::fast::fractal {

void ComputeMandle(float    anXStart,
                   float    aYStart,
                   float    anXEnd,
                   float    aYEnd,
                   int32_t  width,
                   int32_t  height,
                   int32_t  maxIterations,
                   int32_t* output);

}
