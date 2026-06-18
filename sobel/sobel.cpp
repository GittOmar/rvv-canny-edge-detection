#include "sobel.h"
#include <cstdlib>

// Sobel X kernel
static const int16_t KX[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

// Sobel Y kernel
static const int16_t KY[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

GradientImage gradient_alloc(int width, int height) {
    GradientImage g;
    g.width  = width;
    g.height = height;
    // 64-byte aligned for RVV later
    g.Gx = (int16_t*)aligned_alloc(64, width * height * sizeof(int16_t));
    g.Gy = (int16_t*)aligned_alloc(64, width * height * sizeof(int16_t));
    return g;
}

void gradient_free(GradientImage& g) {
    free(g.Gx);
    free(g.Gy);
    g.Gx = g.Gy = nullptr;
}

GradientImage sobel(const Image& src) {
    int W = src.width;
    int H = src.height;
    GradientImage g = gradient_alloc(W, H);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int32_t sumX = 0, sumY = 0;

            for (int ky = 0; ky < 3; ky++) {
                for (int kx = 0; kx < 3; kx++) {
                    int iy = y + ky - 1;
                    int ix = x + kx - 1;

                    // Zero-padding at borders
                    uint8_t pixel = 0;
                    if (iy >= 0 && iy < H && ix >= 0 && ix < W)
                        pixel = src.data[iy * W + ix];

                    sumX += pixel * KX[ky][kx];
                    sumY += pixel * KY[ky][kx];
                }
            }

            g.Gx[y * W + x] = (int16_t)sumX;
            g.Gy[y * W + x] = (int16_t)sumY;
        }
    }

    return g;
}
