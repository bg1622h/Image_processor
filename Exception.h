#pragma once
#include <exception>
#include <string>
class ImageProcessorException : public std::exception {
protected:
    std::string error_message;

public:
    explicit ImageProcessorException(const std::string& error_message);
    const char* what() const noexcept override;
};