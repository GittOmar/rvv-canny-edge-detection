#include "direction.h"
#include <cstdlib>

uint8_t* gradient_direction(const GradientImage& g) {
    int W = g.width;
    int H = g.height;
    uint8_t* dir = (uint8_t*)malloc(W * H);

    for (int i = 0; i < W * H; i++) {
        int32_t ax = abs((int32_t)g.Gx[i]);
        int32_t ay = abs((int32_t)g.Gy[i]);

        // Use integer cross-multiplication instead of atan2()
        // tan(22.5deg) ~ 2/5, tan(67.5deg) ~ 12/5
        // if ay*5 < ax*2  → angle near 0°
        // if ay*5 > ax*12 → angle near 90°
        // otherwise       → angle near 45° or 135°

        uint8_t d;
        if (ay * 5 < ax * 2) {
            d = 0; // 0 degrees (horizontal)
        } else if (ay * 5 > ax * 12) {
            d = 2; // 90 degrees (vertical)
        } else {
            // Determine 45 or 135 based on sign of Gx*Gy
            int32_t sign = g.Gx[i] * g.Gy[i];
            d = (sign > 0) ? 1 : 3;
        }

        dir[i] = d;
    }

    return dir;
}
