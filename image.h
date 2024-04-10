#pragma once
#include <fstream>
#include <vector>
struct Pixel {
    double r = 0;
    double g = 0;
    double b = 0;
    void MakeValid() {
        r = std::clamp(r, 0.0, 1.0);
        g = std::clamp(g, 0.0, 1.0);
        b = std::clamp(b, 0.0, 1.0);
    }
};

class Image {
private:
    std::vector<std::vector<Pixel>> matrix_;
    size_t width_;
    size_t height_;

public:
    Image();
    Image(const size_t height, const size_t width);
    void CropWidth(const size_t width);
    void CropHeight(const size_t height);
    size_t GetHeight() const;
    size_t GetWidth() const;
    Pixel GetPixel(const size_t i, const size_t j) const;
    Pixel& GetPixel(const size_t i, const size_t j);
    void ChangePixel(const size_t i, const size_t j, const Pixel& pixel);
};