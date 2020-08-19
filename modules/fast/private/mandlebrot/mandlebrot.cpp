

#include "mandlebrot.h"

namespace netero::fast::fractal {

void ComputeMandle(float    anXStart,
                   float    aYStart,
                   float    anXEnd,
                   float    aYEnd,
                   int32_t  width,
                   int32_t  height,
                   int32_t  maxIterations,
                   int32_t* output)
{
    ispc::mandelbrot_ispc(anXStart, aYStart, anXEnd, aYEnd, width, height, maxIterations, output);
}

} // namespace netero::fast::fractal
