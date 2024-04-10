#include "request.h"
#include "Exception.h"
#include <cstring>
#include <iostream>
// сначала тут пишу без ошибок
// -crop width height - c +
//-gs +
//-neg +
// -sharp +
// -edge thres
// -blur sigma
Request::Request(int argc, char** argv) {  // потом нам enum перепишу
    try {
        if (argc == 1) {
            return;
        }
        std::vector<char*> arg;
        for (int i = 0; i < argc; ++i) {
            arg.push_back(argv[i]);
        }
        open_path_ = arg.at(1);
        save_path_ = arg.at(2);
        int i = 3;
        while (i < argc) {
            if (std::strcmp(arg.at(i), "-crop") == 0) {
                int width = static_cast<int>(std::stoi(arg.at(i + 1)));
                int height = static_cast<int>(std::stoi(arg.at(i + 2)));
                i += 3;
                commands_.push_back(std::unique_ptr<Filter>(new CropFilter(width, height)));
                continue;
            }
            if (std::strcmp(arg.at(i), "-gs") == 0) {
                commands_.push_back(std::unique_ptr<Filter>(new GrayscaleFilter()));
                ++i;
                continue;
            }
            if (std::strcmp(arg.at(i), "-neg") == 0) {
                commands_.push_back(std::unique_ptr<Filter>(new NegativeFilter()));
                ++i;
                continue;
            }
            if (std::strcmp(arg.at(i), "-sharp") == 0) {
                commands_.push_back(std::unique_ptr<Filter>(new SharpeningFilter()));
                ++i;
                continue;
            }
            if (std::strcmp(arg.at(i), "-edge") == 0) {
                double threshold = std::stod(arg.at(i + 1));
                commands_.push_back(std::unique_ptr<Filter>(new EdgeDetectionFilter(threshold)));
                i += 2;
                continue;
            }
            if (std::strcmp(arg.at(i), "-blur") == 0) {
                double sigma = std::stod(arg.at(i + 1));
                commands_.push_back(std::unique_ptr<Filter>(new GaussFilter(sigma)));
                i += 2;
                continue;
            }
            throw ImageProcessorException("Catch not valid command");
        }
    } catch (const std::invalid_argument& e) {
        throw ImageProcessorException("Invalid arguments for command");
    } catch (const std::out_of_range& e) {
        throw ImageProcessorException("Not enough argument");
    }
}
void Request::Apply(Image& image) {
    for (auto& c : commands_) {
        c->Apply(image);
    }
}
std::string Request::GetOpenpath() {
    return open_path_;
}
std::string Request::GetSavepath() {
    return save_path_;
}