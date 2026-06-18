#include <cstdio>
#include <ctime>
#include "image_io.h"
#include "gaussian.h"
#include "gaussian_rvv.h"
#include "sobel.h"
#include "sobel_rvv.h"
#include "magnitude.h"
#include "direction.h"

double get_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1e6;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Usage: %s input.raw width height output.raw\n", argv[0]);
        return 1;
    }

    const char* input_path  = argv[1];
    int width               = atoi(argv[2]);
    int height              = atoi(argv[3]);
    const char* output_path = argv[4];

    Image src = image_load(input_path, width, height);
    if (!src.data) { printf("Failed to load image\n"); return 1; }

    struct timespec t0, t1, t2, t3, t4;

    // Stage 1: Gaussian blur (RVV)
    clock_gettime(CLOCK_MONOTONIC, &t0);
    Image blurred = gaussian_blur_rvv(src);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    // Stage 2: Sobel (RVV)
    GradientImage grad = sobel_rvv(blurred);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    // Stage 3: Magnitude (scalar)
    Image mag = gradient_magnitude(grad, 0);
    clock_gettime(CLOCK_MONOTONIC, &t3);

    // Stage 4: Direction (scalar)
    uint8_t* dir = gradient_direction(grad);
    clock_gettime(CLOCK_MONOTONIC, &t4);

    image_save(mag, output_path);

    double gaussian_ms = get_ms(t0, t1);
    double sobel_ms    = get_ms(t1, t2);
    double mag_ms      = get_ms(t2, t3);
    double dir_ms      = get_ms(t3, t4);
    double total_ms    = gaussian_ms + sobel_ms + mag_ms + dir_ms;

    printf("=== RVV Timing Results ===\n");
    printf("Gaussian (RVV): %.3f ms (%.1f%%)\n", gaussian_ms, 100*gaussian_ms/total_ms);
    printf("Sobel    (RVV): %.3f ms (%.1f%%)\n", sobel_ms,    100*sobel_ms/total_ms);
    printf("Magnitude:      %.3f ms (%.1f%%)\n", mag_ms,      100*mag_ms/total_ms);
    printf("Direction:      %.3f ms (%.1f%%)\n", dir_ms,      100*dir_ms/total_ms);
    printf("Total:          %.3f ms\n", total_ms);

    image_free(src);
    image_free(blurred);
    gradient_free(grad);
    image_free(mag);
    free(dir);
    return 0;
}
