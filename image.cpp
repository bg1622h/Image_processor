#include "image.h"
#include <iostream>
Image::Image() : matrix_({}), width_(0), height_(0){};
Image::Image(const size_t height, const size_t width)
    : matrix_(std::vector<std::vector<Pixel>>(height, std::vector<Pixel>(width, Pixel()))),
      width_(width),
      height_(height) {
}
void Image::CropHeight(const size_t height_req) {
    matrix_.resize(height_req);
    height_ = height_req;
}
void Image::CropWidth(const size_t width_req) {
    for (auto& row : matrix_) {
        row.resize(width_req);
    }
    width_ = width_req;
}
size_t Image::GetHeight() const {
    return height_;
}
size_t Image::GetWidth() const {
    return width_;
}
Pixel Image::GetPixel(const size_t i, const size_t j) const {
    return matrix_[i][j];
}
Pixel& Image::GetPixel(const size_t i, const size_t j) {
    return matrix_[i][j];
}
void Image::ChangePixel(const size_t i, const size_t j, const Pixel& pixel) {
    matrix_[i][j] = pixel;
}