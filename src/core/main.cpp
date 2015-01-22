#include "core/application.hpp"
#include "core/gettext.hpp" // TODO Not needed anymore if moved

#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    Application app;

    // Internationalization initiation
    // TODO Move in its own file
    setlocale(LC_ALL, "");
    bindtextdomain("eev", "res/po/");
    textdomain("eev");

    // Handle arguments
#if DEBUG_LEVEL >= 1
    if (argc > 1) {
        std::cout << "Arguments passed: " << argv[0] << std::endl;
    }
#endif // DEBUG_LEVEL

    // Running application
    try {
        app.run();
    } catch(std::exception& e) {
        std::cerr << std::endl << "[!] Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
