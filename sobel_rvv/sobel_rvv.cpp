#include "sobel_rvv.h"
#include <riscv_vector.h>
#include <cstdlib>
#include <cstring>

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

GradientImage sobel_rvv(const Image& src) {
    int W = src.width;
    int H = src.height;
    GradientImage g = gradient_alloc(W, H);

    uint8_t* temp = (uint8_t*)malloc(W);

    for (int y = 0; y < H; y++) {
        int x = 0;
        while (x < W) {
            // vl = how many int16 elements fit (LMUL=2)
            size_t vl = __riscv_vsetvl_e16m2(W - x);

            // Accumulators for Gx and Gy
            vint16m2_t accX = __riscv_vmv_v_x_i16m2(0, vl);
            vint16m2_t accY = __riscv_vmv_v_x_i16m2(0, vl);

            for (int ky = 0; ky < 3; ky++) {
                int iy = y + ky - 1;
                for (int kx = 0; kx < 3; kx++) {
                    int16_t cx = KX[ky][kx];
                    int16_t cy = KY[ky][kx];

                    // Skip zero coefficients
                    if (cx == 0 && cy == 0) continue;

                    // Fill temp with boundary-handled pixels
                    for (size_t i = 0; i < vl; i++) {
                        int ix = x + i + kx - 1;
                        if (iy >= 0 && iy < H && ix >= 0 && ix < W)
                            temp[i] = src.data[iy * W + ix];
                        else
                            temp[i] = 0;
                    }

                    // Load 8-bit pixels
                    vuint8m1_t v8 = __riscv_vle8_v_u8m1(temp, vl);

                    // Widen to 16-bit signed
                    vuint16m2_t v16u = __riscv_vwcvtu_x_x_v_u16m2(v8, vl);
                    vint16m2_t  v16s = __riscv_vreinterpret_v_u16m2_i16m2(v16u);

                    // Multiply by kernel coefficient and accumulate
                    if (cx != 0)
                        accX = __riscv_vmacc_vx_i16m2(accX, cx, v16s, vl);
                    if (cy != 0)
                        accY = __riscv_vmacc_vx_i16m2(accY, cy, v16s, vl);
                }
            }

            // Store Gx and Gy results
            __riscv_vse16_v_i16m2(&g.Gx[y * W + x], accX, vl);
            __riscv_vse16_v_i16m2(&g.Gy[y * W + x], accY, vl);

            x += vl;
        }
    }

    free(temp);
    return g;
}
