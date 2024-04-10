#include "BmpImage.h"
#include "Exception.h"
#include <cstring>
#include <iostream>
BmpImage::BmpImage() {
}
BmpImage::BmpImage(std::string path) {
    Open(path);
}  //как раз таки открывает файл
void BmpImage::Open(std::string path) {
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw ImageProcessorException("File could not be opened");
    }
    int file_header_size = bmp_block_sizes::FILE_HEADER_SIZE;
    //т.к offset DIB HEADER = Eh, то размер файла 14
    int info_header_size = bmp_block_sizes::INFO_HEADER_SIZE;
    unsigned char fileheader[file_header_size];
    // надо учесть что читаем по байтам
    file.read(reinterpret_cast<char*>(fileheader), file_header_size);  //что и сколько байт
    if (fileheader[0] != 'B' || fileheader[1] != 'M') {
        throw ImageProcessorException("It's not .bmp file!");
    }

    unsigned char infoheader[info_header_size];
    file.read(reinterpret_cast<char*>(infoheader), info_header_size);  //что и сколько байт

    // int filesize = fileheader[2] + (fileheader[3] << 8) + (fileheader[4] << 16) + (fileheader[5] << 24);
    int cur_pos = 4;

    size_t bi_width = infoheader[cur_pos] + (infoheader[cur_pos + 1] << erase_bites::ONE) +
                      (infoheader[cur_pos + 2] << erase_bites::TWO) + (infoheader[cur_pos + 3] << erase_bites::THREE);
    cur_pos += 4;
    size_t bi_height = infoheader[cur_pos] + (infoheader[cur_pos + 1] << erase_bites::ONE) +
                       (infoheader[cur_pos + 2] << erase_bites::TWO) + (infoheader[cur_pos + 3] << erase_bites::THREE);
    image_ = Image(bi_height, bi_width);
    const int padding = ((4 - (static_cast<int>(bi_width) * 3) % 4) % 4);
    for (size_t i = 0; i < bi_height; ++i) {
        for (size_t j = 0; j < bi_width; ++j) {
            unsigned char color[3];
            file.read(reinterpret_cast<char*>(color), 3);  // bgr
            Pixel new_pixel;
            new_pixel.b = static_cast<double>(color[0]) / MAXCOLORVALUE;
            new_pixel.g = static_cast<double>(color[1]) / MAXCOLORVALUE;
            new_pixel.r = static_cast<double>(color[2]) / MAXCOLORVALUE;
            image_.ChangePixel(bi_height - i - 1, j, new_pixel);
        }
        file.ignore(padding);
    }
    file.close();
}
void BmpImage::Save(std::string path) {
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        throw ImageProcessorException("File could not be opened");
    }
    unsigned char pad_array[3] = {0, 0, 0};
    const int padding = ((4 - (static_cast<int>(image_.GetWidth()) * 3) % 4) % 4);
    const int file_header_size = bmp_block_sizes::FILE_HEADER_SIZE;
    //т.к offset DIB HEADER = Eh, то размер файла 14
    const int info_header_size = bmp_block_sizes::INFO_HEADER_SIZE;
    const unsigned int filesize =
        file_header_size + info_header_size + image_.GetHeight() * image_.GetWidth() * 3 + padding * image_.GetHeight();

    unsigned char fileheader[file_header_size];
    std::memset(fileheader, 0, sizeof(fileheader));
    int cur_pos = 0;
    // type
    fileheader[cur_pos] = 'B';
    fileheader[cur_pos + 1] = 'M';
    cur_pos += 2;
    // size
    fileheader[cur_pos] = filesize;
    fileheader[cur_pos + 1] = filesize >> erase_bites::ONE;
    fileheader[cur_pos + 2] = filesize >> erase_bites::TWO;
    fileheader[cur_pos + 3] = filesize >> erase_bites::THREE;
    cur_pos += 4;
    // reserve 1
    // reserve 2
    // offset
    fileheader[cur_pos] = file_header_size + info_header_size;
    ++cur_pos;
    unsigned char infoheader[info_header_size];
    std::memset(infoheader, 0, sizeof(infoheader));
    cur_pos = 0;
    // infoheader size
    infoheader[cur_pos] = info_header_size;
    // image width
    unsigned int width = image_.GetWidth();
    cur_pos = 4;
    infoheader[cur_pos] = width;
    infoheader[cur_pos + 1] = width >> erase_bites::ONE;
    infoheader[cur_pos + 2] = width >> erase_bites::TWO;
    infoheader[cur_pos + 3] = width >> erase_bites::THREE;
    cur_pos += 4;
    unsigned int height = image_.GetHeight();
    // image height
    infoheader[cur_pos] = height;
    infoheader[cur_pos + 1] = height >> erase_bites::ONE;
    infoheader[cur_pos + 2] = height >> erase_bites::TWO;
    infoheader[cur_pos + 3] = height >> erase_bites::THREE;
    cur_pos += 4;
    infoheader[cur_pos] = 1;
    infoheader[cur_pos + 1] = 0;
    cur_pos += 2;
    // bits for pixel
    infoheader[cur_pos] = bmp_block_sizes::BIT_FOR_PIXEL;
    infoheader[cur_pos + 1] = 0;
    cur_pos += 2;
    // no compress
    // image size(no compress)
    // Надо помнить, что изоображение хранится с левого нижнего угла
    file.write(reinterpret_cast<char*>(fileheader), file_header_size);
    file.write(reinterpret_cast<char*>(infoheader), info_header_size);
    for (size_t i = 0; i < image_.GetHeight(); ++i) {
        for (size_t j = 0; j < image_.GetWidth(); ++j) {
            Pixel cur_pixel = image_.GetPixel(image_.GetHeight() - i - 1, j);
            unsigned char r = static_cast<unsigned char>(cur_pixel.r * MAXCOLORVALUE);
            unsigned char g = static_cast<unsigned char>(cur_pixel.g * MAXCOLORVALUE);
            unsigned char b = static_cast<unsigned char>(cur_pixel.b * MAXCOLORVALUE);

            unsigned char color[3] = {b, g, r};
            file.write(reinterpret_cast<char*>(color), 3);
        }
        file.write(reinterpret_cast<char*>(pad_array), padding);
    }
    file.close();
}
Image& BmpImage::GetAllImage() {
    return image_;
}