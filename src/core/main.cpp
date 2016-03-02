#include "core/application.hpp"
#include "core/gettext.hpp"
#include "core/debug.hpp"
#include "tools/time.hpp"
#include "tools/random.hpp"
#include "tools/filesystem.hpp"

#include <steam/steam.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h> // STDERR_FILENO

#if defined(__GNUC__)
#if not defined(__MINGW32__)
#include <signal.h>

extern "C"
{
    //! Inject the callstack generation each time a signal is lauched (SEGFAULT, etc.)
    void handler(int sig)
    {
        mquit(strsignal(sig));
    }
}
#endif
#endif

//! Main function.
int main(int argc, char *argv[])
{
    //----- Bugs catching -----//

    // Catch GCC signals
    #if defined(__GNUC__)
    #if not defined(__MINGW32__)
    signal(SIGSEGV, handler);
    signal(SIGBUS, handler);
    signal(SIGILL, handler);
    signal(SIGSYS, handler);
    signal(SIGFPE, handler);
    #endif
    #endif

    //----- Initialization -----//

    // Logs
    createDirectory(L"log");

    // Arguments
    std::vector<std::string> args(argc - 1);
    for (uint i = 0u; i < args.size(); ++i)
        args[i] = argv[i + 1u];

    // Initialize randomness
    alea::s_generator.seed(time(nullptr));

    // As we do not control all libraries,
    // keeping in sync is important to avoid interleaved characters.
    std::ios_base::sync_with_stdio(true);
    std::wcout << L"";
    std::cerr << "";

    //----- Steam API -----//

    // Redirecting stdcerr (C interface) to file while Steam is loading
    auto steamErrorFileName = "log/steam_" + time2string("%Y%m%d-%H%M%S") + ".log";
    auto steamErrorFile = fopen(steamErrorFileName.c_str(), "w+");
    auto stdcerrOriginal = dup(STDERR_FILENO);
    dup2(fileno(steamErrorFile), STDERR_FILENO);

    // Start Steam
    // Note: That's important to do that before the call to Application,
    // otherwise the hook to OpenGL (Shift+Tab) won't work.
    bool steamActivated = steam::SteamAPI_Init();

    // Setting cerr back
    fflush(stderr);
    dup2(stdcerrOriginal, STDERR_FILENO);
    fclose(steamErrorFile);
    close(stdcerrOriginal);

    // Steam state
    mdebug_core_1("Steam interfacing: " << (steamActivated? "Enabled" : "Disabled"));
    if (!steamActivated) mdebug_core_1("Steam loading errors have been saved to steam.log");

    //----- Application -----//

    Application app(args);
    app.run();
    mdebug_core_1("Quitting game safely.")

    //----- Closing procedures -----//

    // Closing Steam
    if (steamActivated)
        steam::SteamAPI_Shutdown();

    // Free internationalization
    i18n::close();

    return EXIT_SUCCESS;
}

