#include "BmpImage.h"
#include "filters.h"
#include "request.h"
#include <iostream>
#include <cstdlib>
int main(int argc, char** argv) {
    try {
        auto parser_out = Request(argc, argv);
        if (argc > 1) {
            BmpImage working_image(parser_out.GetOpenpath());
            parser_out.Apply(working_image.GetAllImage());
            working_image.Save(parser_out.GetSavepath());
        } else {
            Info();
        }
        return EXIT_SUCCESS;
    } catch (const std::exception& exception) {
        std::cout << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
}
