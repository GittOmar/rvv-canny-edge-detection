#pragma once
#include "image_io.h"
#include "sobel.h"

// Compute gradient magnitude and normalize to [0,255]
// method 0 = L1 norm (|Gx|+|Gy|), method 1 = L2 norm (sqrt)
Image gradient_magnitude(const GradientImage& g, int method);
