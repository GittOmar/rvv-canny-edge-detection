#include "image_io.h"
#include <cstdio>
#include <cstdlib>

Image image_alloc(int width, int height) {
    Image img;
    img.width  = width;
    img.height = height;
    img.data = (uint8_t*)aligned_alloc(64, width * height);   دي بتجبر الكمبيوتر يحجز مكان في الذاكرة يبدأ من عنوان يقبل القسمة على 64 بايت.
    return img;
}

void image_free(Image& img) {
    free(img.data);
    img.data = nullptr;
}

Image image_load(const char* path, int width, int height) {
    Image img = image_alloc(width, height);
    FILE* f = fopen(path, "rb");
    if (!f) { img.data = nullptr; return img; }
    fread(img.data, 1, width * height, f);
    fclose(f);
    return img;
}

void image_save(const Image& img, const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) return;
    fwrite(img.data, 1, img.width * img.height, f);
    fclose(f);
}
