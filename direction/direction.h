#pragma once
#include "image_io.h"
#include "sobel.h"

// Returns a buffer of direction values (0,1,2,3) for each pixel
// 0=0deg, 1=45deg, 2=90deg, 3=135deg
// Caller must free() the returned buffer
uint8_t* gradient_direction(const GradientImage& g);
