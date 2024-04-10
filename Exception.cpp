#include "Exception.h"
ImageProcessorException::ImageProcessorException(const std::string& error_message) : error_message(error_message) {
}
const char* ImageProcessorException::what() const noexcept {
    return error_message.c_str();
}