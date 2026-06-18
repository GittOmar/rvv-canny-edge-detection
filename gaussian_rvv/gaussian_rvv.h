#pragma once
#include "image_io.h"

// RVV-optimized Gaussian blur
// Produces identical output to gaussian_blur() but uses vector instructions
Image gaussian_blur_rvv(const Image& src);
