#pragma once
#include <cstdint>
#include <cstdlib>

struct Image {
    uint8_t* data;
    int width;
    int height;
};

Image image_alloc(int width, int height);
void image_free(Image& img);
Image image_load(const char* path, int width, int height);
void image_save(const Image& img, const char* path);
