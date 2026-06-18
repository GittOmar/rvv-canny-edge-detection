#pragma once
#include "sobel.h"

// RVV-optimized Sobel gradients
// Produces identical output to sobel() but uses vector instructions
GradientImage sobel_rvv(const Image& src);
