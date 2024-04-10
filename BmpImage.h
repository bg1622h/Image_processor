#pragma once
#include "image.h"
namespace erase_bites {
const int ONE = 8;
const int TWO = 16;
const int THREE = 24;
}  // namespace erase_bites
namespace bmp_block_sizes {
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;
const int BIT_FOR_PIXEL = 24;
}  // namespace bmp_block_sizes
const double MAXCOLORVALUE = 255.0;
class BmpImage {
private:
    Image image_;

public:
    BmpImage();
    explicit BmpImage(std::string path);
    void Clear();
    void Open(std::string path);
    void Save(std::string path);
    Image& GetAllImage();
};