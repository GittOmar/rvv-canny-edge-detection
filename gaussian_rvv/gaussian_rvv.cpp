#include "gaussian_rvv.h"
#include <riscv_vector.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// Same kernel as scalar version (sum=273)
static const int16_t KERNEL[5][5] = {
    { 1,  4,  7,  4,  1},
    { 4, 16, 26, 16,  4},
    { 7, 26, 41, 26,  7},
    { 4, 16, 26, 16,  4},
    { 1,  4,  7,  4,  1}
};
static const int32_t KERNEL_SUM = 273;
static const int HALF = 2;

Image gaussian_blur_rvv(const Image& src) {
    Image dst = image_alloc(src.width, src.height);
    int W = src.width;
    int H = src.height;

    // Temp buffer for boundary-handled pixels
    uint8_t* temp = (uint8_t*)malloc(W);

    for (int y = 0; y < H; y++) {
        int x = 0;
        while (x < W) {
            // Ask hardware how many int32 elements fit (LMUL=2)
            size_t vl = __riscv_vsetvl_e32m2(W - x);

            // Initialize accumulator to zero
            vint32m2_t acc = __riscv_vmv_v_x_i32m2(0, vl);

            for (int ky = 0; ky < 5; ky++) {
                int iy = y + ky - HALF;
                for (int kx = 0; kx < 5; kx++) {
                    int16_t coeff = KERNEL[ky][kx];

                    // Fill temp buffer with boundary-handled pixels
                    for (size_t i = 0; i < vl; i++) {
                        int ix = x + i + kx - HALF;
                        if (iy >= 0 && iy < H && ix >= 0 && ix < W)
                            temp[i] = src.data[iy * W + ix];
                        else
                            temp[i] = 0;
                    }

                    // Load 8-bit pixels into vector
                    vuint8m1_t v8 = __riscv_vle8_v_u8m1(temp, vl);

                    // Widen 8-bit to 16-bit
                    vuint16m2_t v16u = __riscv_vwcvtu_x_x_v_u16m2(v8, vl);
                    vint16m2_t  v16s = __riscv_vreinterpret_v_u16m2_i16m2(v16u);

                    // Widen multiply: 16-bit * scalar coeff -> 32-bit
                    vint32m4_t prod32 = __riscv_vwmul_vx_i32m4(v16s, coeff, vl);

                    // Truncate LMUL: m4 -> m2 (safe because values fit)
                    vint32m2_t prod = __riscv_vlmul_trunc_v_i32m4_i32m2(prod32);

                    // Accumulate
                    acc = __riscv_vadd_vv_i32m2(acc, prod, vl);
                }
            }

            // Add rounding offset, then divide by 273
            // Using fixed-point: (x * 240) >> 16 ≈ x / 273
            acc = __riscv_vadd_vx_i32m2(acc, KERNEL_SUM / 2, vl);
            vint32m2_t result = __riscv_vmul_vx_i32m2(acc, 240, vl);
            result = __riscv_vsra_vx_i32m2(result, 16, vl);

            // Clamp to [0, 255]
            result = __riscv_vmax_vx_i32m2(result, 0, vl);
            result = __riscv_vmin_vx_i32m2(result, 255, vl);

            // Narrow 32-bit -> 8-bit and store
            vint16m1_t n16 = __riscv_vnsra_wx_i16m1(result, 0, vl);
            vuint8mf2_t n8 = __riscv_vnclipu_wx_u8mf2(
                __riscv_vreinterpret_v_i16m1_u16m1(n16), 0, 0, vl);
            __riscv_vse8_v_u8mf2(&dst.data[y * W + x], n8, vl);

            x += vl;
        }
    }

    free(temp);
    return dst;
}
