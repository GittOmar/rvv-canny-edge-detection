#include <gtest/gtest.h>
#include "image_io.h"
#include "test_image_gen.h"
#include "gaussian.h"
#include "sobel.h"
#include "magnitude.h"
#include "direction.h"

// ── Gaussian Tests ────────────────────────────────────────

TEST(GaussianTest, UniformImageUnchanged) {
    // Blurring a uniform image should keep it uniform
    Image img = gen_uniform(64, 64, 128);
    Image blurred = gaussian_blur(img);
    // Check interior pixels only (avoid border effects)
    for (int y = 5; y < 59; y++)
        for (int x = 5; x < 59; x++)
            EXPECT_NEAR(blurred.data[y*64+x], 128, 3);
    image_free(img);
    image_free(blurred);
}

TEST(GaussianTest, BlackImageStaysBlack) {
    Image img = gen_black(64, 64);
    Image blurred = gaussian_blur(img);
    for (int i = 0; i < 64*64; i++)
        EXPECT_EQ(blurred.data[i], 0);
    image_free(img);
    image_free(blurred);
}

TEST(GaussianTest, BlurReducesSharpEdge) {
    // After blurring a sharp edge, border pixels should be softer
    Image img = gen_vertical_edge(64, 64);
    Image blurred = gaussian_blur(img);
    // The pixel at the edge center should be between 0 and 255
    int mid = 32 * 64 + 32;
    EXPECT_GT(blurred.data[mid], 0);
    EXPECT_LT(blurred.data[mid], 255);
    image_free(img);
    image_free(blurred);
}

// ── Sobel Tests ───────────────────────────────────────────

TEST(SobelTest, UniformImageZeroGradient) {
    // Uniform image has no edges
    Image img = gen_uniform(64, 64, 128);
    GradientImage g = sobel(img);
    for (int y = 2; y < 62; y++)
        for (int x = 2; x < 62; x++) {
            int i = y*64+x;
            EXPECT_EQ(g.Gx[i], 0);
            EXPECT_EQ(g.Gy[i], 0);
        }
    image_free(img);
    gradient_free(g);
}

TEST(SobelTest, VerticalEdgeLargeGx) {
    // Vertical edge: large Gx, small Gy in the middle
    Image img = gen_vertical_edge(64, 64);
    GradientImage g = sobel(img);
    // Check middle row, at the edge column
    int mid_row = 32;
    int edge_col = 32;
    int idx = mid_row * 64 + edge_col;
    EXPECT_GT(abs(g.Gx[idx]), 100);
    EXPECT_LT(abs(g.Gy[idx]), 10);
    image_free(img);
    gradient_free(g);
}

TEST(SobelTest, HorizontalEdgeLargeGy) {
    // Horizontal edge: large Gy, small Gx in the middle
    Image img = gen_horizontal_edge(64, 64);
    GradientImage g = sobel(img);
    int mid_col = 32;
    int edge_row = 32;
    int idx = edge_row * 64 + mid_col;
    EXPECT_GT(abs(g.Gy[idx]), 100);
    EXPECT_LT(abs(g.Gx[idx]), 10);
    image_free(img);
    gradient_free(g);
}

// ── Magnitude Tests ───────────────────────────────────────

TEST(MagnitudeTest, NonzeroOnEdgeImage) {
    Image img = gen_vertical_edge(64, 64);
    GradientImage g = sobel(img);
    Image mag = gradient_magnitude(g, 0); // L1
    int nonzero = 0;
    for (int i = 0; i < 64*64; i++)
        if (mag.data[i] > 0) nonzero++;
    EXPECT_GT(nonzero, 0);
    image_free(img);
    gradient_free(g);
    image_free(mag);
}

TEST(MagnitudeTest, L1AndL2BothWork) {
    Image img = gen_rectangle(64, 64);
    GradientImage g = sobel(img);
    Image mag_l1 = gradient_magnitude(g, 0);
    Image mag_l2 = gradient_magnitude(g, 1);
    EXPECT_EQ(mag_l1.data == nullptr, false);
    EXPECT_EQ(mag_l2.data == nullptr, false);
    image_free(img);
    gradient_free(g);
    image_free(mag_l1);
    image_free(mag_l2);
}

// ── Direction Tests ───────────────────────────────────────

TEST(DirectionTest, VerticalEdgeIsHorizontalGradient) {
    // Vertical edge → gradient points horizontally → direction = 0
    Image img = gen_vertical_edge(64, 64);
    GradientImage g = sobel(img);
    uint8_t* dir = gradient_direction(g);
    // Check middle of the edge
    int idx = 32 * 64 + 32;
    EXPECT_EQ(dir[idx], 0);
    image_free(img);
    gradient_free(g);
    free(dir);
}

TEST(DirectionTest, HorizontalEdgeIsVerticalGradient) {
    // Horizontal edge → gradient points vertically → direction = 2
    Image img = gen_horizontal_edge(64, 64);
    GradientImage g = sobel(img);
    uint8_t* dir = gradient_direction(g);
    int idx = 32 * 64 + 32;
    EXPECT_EQ(dir[idx], 2);
    image_free(img);
    gradient_free(g);
    free(dir);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
