#pragma once
#include "image.h"
#include <math.h>
const double PI = std::acos(-1);
namespace grayscale_koef {
const double R = 0.299;
const double G = 0.587;
const double B = 0.114;
}  // namespace grayscale_koef
namespace matrices_for_filters {
const std::vector<std::vector<double>> SHARP{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
const std::vector<std::vector<double>> EDGE_DETECTION{{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
}  // namespace matrices_for_filters
class Filter {
public:
    bool write_info_ = true;
    virtual void Apply(Image& matrix){};
    Filter(){};
    virtual ~Filter(){};
    bool GetInfoFlag() const;
};
class CropFilter : public Filter {
private:
    int width_ = 0;
    int height_ = 0;

public:
    CropFilter(const int width, const int height);
    void Apply(Image& matrix) override;
    static void Info();
};
class GrayscaleFilter : public Filter {
public:
    explicit GrayscaleFilter(bool info_flag = false);
    void Apply(Image& matrix) override;
    static void Info();
};
class NegativeFilter : public Filter {
public:
    explicit NegativeFilter(bool info_flag = false);
    void Apply(Image& matrix) override;
    static void Info();
};

class MatrixFilter : public Filter {
protected:
    std::vector<std::vector<double>> filter;
    size_t filter_size = 0;

public:
    MatrixFilter(){};
    explicit MatrixFilter(std::vector<std::vector<double>> filter);
    void Apply(Image& matrix) override;
};
class SharpeningFilter : public MatrixFilter {
public:
    explicit SharpeningFilter(const bool info_flag = false);
    static void Info();
};
class EdgeDetectionFilter : public MatrixFilter {
private:
    GrayscaleFilter to_gray_;
    long double threshold_;

public:
    explicit EdgeDetectionFilter(long double threshold, const bool info_flag = false);
    void Apply(Image& matrix) override;
    static void Info();
};
namespace gauss_constants {
const double FILTER_SIZE = 6.0;
const double SIGMA_KOEF = 2.0;
const double STEP_KOEF = 3.0;
}  // namespace gauss_constants
class GaussFilter : public Filter {
private:
    double sigma_;
    int step_;

public:
    explicit GaussFilter(double sigma, const bool info_flag = false);
    static void Info();
    void Apply(Image& matrix) override;
};
void Info();