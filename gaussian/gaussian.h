#pragma once
#include "image_io.h"

// Gaussian blur with 5x5 kernel, sigma~1.0
// Uses integer arithmetic, zero-padding at borders
// Output is a new Image (caller must free it)
Image gaussian_blur(const Image& src);
