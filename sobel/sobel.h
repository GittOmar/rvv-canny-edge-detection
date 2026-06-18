#pragma once
#include "image_io.h"

// Holds the two gradient images
struct GradientImage {
    int16_t* Gx;   // horizontal gradient
    int16_t* Gy;   // vertical gradient
    int width;
    int height;
};

// Allocate gradient buffers
GradientImage gradient_alloc(int width, int height);

// Free gradient buffers
void gradient_free(GradientImage& g);

// Compute Sobel gradients from blurred image
GradientImage sobel(const Image& src);
