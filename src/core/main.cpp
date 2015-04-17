#include "core/application.hpp"
#include "core/gettext.hpp"

#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    Application app;
    internationalization::init("");

    // Disable synchronisation
    // Note: Remove the safety of using printf/scanf
    // This makes the streams faster
    std::ios_base::sync_with_stdio(false);

    // Handle arguments
    for (int i = 1; i < argc; ++i)
        mdebug_application_1("Argument: " << argv[i]);

    // Running application
    try {
        app.run();
    } catch(std::exception& e) {
        std::cerr << std::endl << "[!] Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

