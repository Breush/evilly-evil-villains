#include "core/application.hpp"
#include "core/gettext.hpp"

#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    Application app;
    internationalization::init();

    // Handle arguments
    if (argc > 1)
        mdebug_application_1("First argument passed: " << argv[0]);

    // Running application
    try {
        app.run();
    } catch(std::exception& e) {
        std::cerr << std::endl << "[!] Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
