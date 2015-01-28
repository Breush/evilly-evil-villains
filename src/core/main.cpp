#include "core/application.hpp"
#include "core/gettext.hpp"

#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    Application app;
    internationalization::init();

    // Handle arguments
    debug_1(if (argc > 1) std::cout << "Arguments passed: " << argv[0] << std::endl;)

    // Running application
    try {
        app.run();
    } catch(std::exception& e) {
        std::cerr << std::endl << "[!] Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
