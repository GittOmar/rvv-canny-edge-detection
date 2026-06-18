#include "magnitude.h"
#include <cmath>
#include <cstdlib>

Image gradient_magnitude(const GradientImage& g, int method) {
    int W = g.width;
    int H = g.height;
    Image dst = image_alloc(W, H);

    // Pass 1: compute raw magnitudes into a temp buffer
    int32_t* mag = (int32_t*)malloc(W * H * sizeof(int32_t));
    int32_t max_mag = 0;

    for (int i = 0; i < W * H; i++) {
        int32_t gx = g.Gx[i];
        int32_t gy = g.Gy[i];
        int32_t m;

        if (method == 0) {
            // L1 norm: |Gx| + |Gy|
            m = abs(gx) + abs(gy);
        } else {
            // L2 norm: sqrt(Gx^2 + Gy^2)
            m = (int32_t)sqrtf((float)(gx*gx + gy*gy));
        }

        mag[i] = m;
        if (m > max_mag) max_mag = m;
    }

    // Pass 2: normalize to [0,255]
    for (int i = 0; i < W * H; i++) {
        if (max_mag == 0)
            dst.data[i] = 0;
        else
            dst.data[i] = (uint8_t)(mag[i] * 255 / max_mag);
    }

    free(mag);
    return dst;
}
