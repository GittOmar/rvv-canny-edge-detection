#include "gaussian.h"
#include <cstdint>

// 5x5 Gaussian kernel coefficients (sum = 273)
static const int16_t KERNEL[5][5] = {
    { 1,  4,  7,  4,  1},
    { 4, 16, 26, 16,  4},
    { 7, 26, 41, 26,  7},
    { 4, 16, 26, 16,  4},
    { 1,  4,  7,  4,  1}
};
static const int KERNEL_SUM = 273;
static const int KERNEL_SIZE = 5;
static const int HALF = 2; // half of 5x5

 //src -> Source image (before)
// dst -> Destination image (after)
Image gaussian_blur(const Image& src) {
    Image dst = image_alloc(src.width, src.height); // dst -> Destination
    int W = src.width;
    int H = src.height;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int32_t sum = 0;

            // Apply 5x5 kernel
            for (int ky = 0; ky < KERNEL_SIZE; ky++) {
                for (int kx = 0; kx < KERNEL_SIZE; kx++) {
                    int iy = y + ky - HALF;
                    int ix = x + kx - HALF;

                    // Zero-padding: out of bounds pixels = 0
                    uint8_t pixel = 0;
                    if (iy >= 0 && iy < H && ix >= 0 && ix < W)
                        pixel = src.data[iy * W + ix];

                    sum += pixel * KERNEL[ky][kx];
                }
            }

            // Divide by kernel sum and clamp to [0,255]
            // Add KERNEL_SUM/2 before dividing for correct rounding
            int32_t result = (sum + KERNEL_SUM/2) / KERNEL_SUM;
            if (result < 0)   result = 0;
            if (result > 255) result = 255;
            dst.data[y * W + x] = (uint8_t)result;
        }
    }

    return dst;
}
