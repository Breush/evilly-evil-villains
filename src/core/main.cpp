#include "core/application.hpp"
#include "core/gettext.hpp"
#include "core/debug.hpp"

#include <steam/steam.hpp>

#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    Application app;
    int returnStatus = EXIT_SUCCESS;

    // Disable synchronisation
    // Note: Remove the safety of using printf/scanf (so don't use them)
    // but this makes the streams faster.
    std::ios_base::sync_with_stdio(false);

    // Handle arguments
    for (int i = 1; i < argc; ++i)
        mdebug_core_1("Argument " << i << " : " << argv[i]);

    // Start Steam
    bool steamActivated = false;
    // No need to try: there nothing to activate unless we get an appID,
    // which supposed to have been greenlit.
    // steamActivated = steam::SteamAPI_Init();
    mdebug_core_1("Steam interfacing: " << (steamActivated? "Enabled" : "Disabled"));

    // Running application
    try {
        app.run();
    }
    catch(std::exception& e) {
        std::cerr << std::endl << "[!] Exception caught: " << e.what() << std::endl;
        returnStatus = EXIT_FAILURE;
    }

    // Closing Steam
    if (steamActivated)
        steam::SteamAPI_Shutdown();

    // Free internationalization
    internationalization::close();

    return returnStatus;
}

