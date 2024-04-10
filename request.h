#pragma once
#include "filters.h"
#include <memory>
class Request {
private:
    std::vector<std::unique_ptr<Filter>> commands_;
    std::string open_path_;
    std::string save_path_;

public:
    Request(int argc, char** argv);
    void Apply(Image& image);
    std::string GetOpenpath();
    std::string GetSavepath();
};