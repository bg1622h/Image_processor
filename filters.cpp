#include "filters.h"
#include "Exception.h"
#include <iostream>
//кроме как crop в фильтрах не вижу проблем
bool Filter::GetInfoFlag() const {
    return write_info_;
}
CropFilter::CropFilter(const int width, const int height) : width_(width), height_(height) {
    write_info_ = false;
}
void CropFilter::Apply(Image& matrix) {
    if (width_ <= 0 || height_ <= 0) {
        throw ImageProcessorException("Invalid image size");
    }
    matrix.CropHeight(std::min(matrix.GetHeight(), static_cast<size_t>(height_)));
    matrix.CropWidth(std::min(matrix.GetWidth(), static_cast<size_t>(width_)));
}
void CropFilter::Info() {
    std::cout << "-crop width height\nCrops the image to the width * height dimensions" << std::endl;
}
GrayscaleFilter::GrayscaleFilter(bool info_flag) {
    write_info_ = info_flag;
}
void GrayscaleFilter::Apply(Image& matrix) {
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            Pixel& current_pixel = matrix.GetPixel(i, j);
            double pixel_value = (grayscale_koef::R * current_pixel.r) + (grayscale_koef::G * current_pixel.g) +
                                 (grayscale_koef::B * current_pixel.b);
            current_pixel.r = pixel_value;
            current_pixel.g = pixel_value;
            current_pixel.b = pixel_value;
        }
    }
}
void GrayscaleFilter::Info() {
    std::cout << "-gs\nConverts the image to grayscale" << std::endl;
}
NegativeFilter::NegativeFilter(bool info_flag) {
    write_info_ = info_flag;
}
void NegativeFilter::Apply(Image& matrix) {
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            Pixel current_pixel = matrix.GetPixel(i, j);
            current_pixel.r = 1 - current_pixel.r;
            current_pixel.g = 1 - current_pixel.g;
            current_pixel.b = 1 - current_pixel.b;
            matrix.ChangePixel(i, j, current_pixel);
        }
    }
}
void NegativeFilter::Info() {
    std::cout << "-neg\nConverts an image to a negative using the formula" << std::endl;
}

// считаю что фильтры квадратные
MatrixFilter::MatrixFilter(std::vector<std::vector<double>> filter) : filter(filter), filter_size(filter.size()) {
}
void MatrixFilter::Apply(Image& matrix) {
    Image result_image(matrix.GetHeight(), matrix.GetWidth());
    int half_filter_size = static_cast<int>(filter_size) / 2;
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            Pixel newpixel_value = {0, 0, 0};
            for (size_t fi = 0; fi < filter_size; ++fi) {
                for (size_t fj = 0; fj < filter_size; ++fj) {
                    int target_pixeli = static_cast<int>(i) + (static_cast<int>(fi) - half_filter_size);
                    int target_pixelj = static_cast<int>(j) + (static_cast<int>(fj) - half_filter_size);
                    // берём ближайшие пиксели
                    target_pixeli = std::clamp(target_pixeli, 0, static_cast<int>(matrix.GetHeight()) - 1);
                    target_pixelj = std::clamp(target_pixelj, 0, static_cast<int>(matrix.GetWidth()) - 1);
                    newpixel_value.r +=
                        filter[fi][fj] *
                        matrix.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).r;
                    newpixel_value.g +=
                        filter[fi][fj] *
                        matrix.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).g;
                    newpixel_value.b +=
                        filter[fi][fj] *
                        matrix.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).b;
                }
            }
            newpixel_value.MakeValid();
            result_image.ChangePixel(i, j, newpixel_value);
        }
    }
    matrix = result_image;
}
SharpeningFilter::SharpeningFilter(const bool info_flag) : MatrixFilter(matrices_for_filters::SHARP) {
    write_info_ = info_flag;
}
void SharpeningFilter::Info() {
    std::cout << "-sharp\nIncreases the sharpness of the image" << std::endl;
}
EdgeDetectionFilter::EdgeDetectionFilter(long double threshold, const bool info_flag)
    : MatrixFilter(matrices_for_filters::EDGE_DETECTION), threshold_(threshold) {
    write_info_ = info_flag;
}
void EdgeDetectionFilter::Apply(Image& matrix) {
    to_gray_.Apply(matrix);
    MatrixFilter::Apply(matrix);  // посмотри что применяется (должна матрица с центром 4-й)
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            Pixel current_pixel = matrix.GetPixel(i, j);
            if (current_pixel.r > threshold_) {
                current_pixel.r = 1;
                current_pixel.g = 1;
                current_pixel.b = 1;
            } else {
                current_pixel.r = 0;
                current_pixel.g = 0;
                current_pixel.b = 0;
            }
            matrix.ChangePixel(i, j, current_pixel);
        }
    }
}
void EdgeDetectionFilter::Info() {
    std::cout << "-edge threshold\nHighlights the borders of the image" << std::endl;
}

GaussFilter::GaussFilter(double sigma, const bool info_flag) : sigma_(sigma) {
    write_info_ = info_flag;
    step_ = static_cast<int>(std::ceil(gauss_constants::STEP_KOEF * sigma_)) + 1;
}
void GaussFilter::Info() {
    std::cout << "-blur sigma\nApplies Gaussian blur to an image with sigma parameter" << std::endl;
}
void GaussFilter::Apply(Image& matrix) {
    Image result_image(matrix.GetHeight(), matrix.GetWidth());
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            Pixel newpixel_value = {0, 0, 0};
            for (int fj = -step_; fj < step_; ++fj) {
                int target_pixeli = static_cast<int>(i);
                int target_pixelj = static_cast<int>(j) + fj;
                double diff = static_cast<double>(fj);
                double koef = std::exp(-(diff * diff) / (sigma_ * sigma_ * gauss_constants::SIGMA_KOEF));
                target_pixeli = std::clamp(target_pixeli, 0, static_cast<int>(matrix.GetHeight()) - 1);
                target_pixelj = std::clamp(target_pixelj, 0, static_cast<int>(matrix.GetWidth()) - 1);
                newpixel_value.r +=
                    koef * matrix.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).r;
                newpixel_value.g +=
                    koef * matrix.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).g;
                newpixel_value.b +=
                    koef * matrix.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).b;
            }
            result_image.ChangePixel(i, j, newpixel_value);
        }
    }
    Image real_result_image(matrix.GetHeight(), matrix.GetWidth());
    for (size_t i = 0; i < matrix.GetHeight(); ++i) {
        for (size_t j = 0; j < matrix.GetWidth(); ++j) {
            Pixel newpixel_value = {0, 0, 0};
            for (int fi = -step_; fi < step_; ++fi) {
                int target_pixelj = static_cast<int>(j);
                int target_pixeli = static_cast<int>(i) + fi;
                double diff = static_cast<double>(fi);
                double koef = std::exp(-(diff * diff) / (sigma_ * sigma_ * gauss_constants::SIGMA_KOEF));
                // берём ближайшие пиксели
                target_pixeli = std::clamp(target_pixeli, 0, static_cast<int>(matrix.GetHeight()) - 1);
                target_pixelj = std::clamp(target_pixelj, 0, static_cast<int>(matrix.GetWidth()) - 1);
                newpixel_value.r +=
                    koef *
                    result_image.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).r;
                newpixel_value.g +=
                    koef *
                    result_image.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).g;
                newpixel_value.b +=
                    koef *
                    result_image.GetPixel(static_cast<size_t>(target_pixeli), static_cast<size_t>(target_pixelj)).b;
            }
            newpixel_value.r /= (PI * sigma_ * sigma_ * gauss_constants::SIGMA_KOEF);
            newpixel_value.g /= (PI * sigma_ * sigma_ * gauss_constants::SIGMA_KOEF);
            newpixel_value.b /= (PI * sigma_ * sigma_ * gauss_constants::SIGMA_KOEF);
            newpixel_value.MakeValid();
            real_result_image.ChangePixel(i, j, newpixel_value);
        }
    }
    matrix = real_result_image;
}
void Info() {
    CropFilter::Info();
    GrayscaleFilter::Info();
    NegativeFilter::Info();
    SharpeningFilter::Info();
    EdgeDetectionFilter::Info();
    GaussFilter::Info();
}