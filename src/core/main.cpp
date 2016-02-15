#include "core/application.hpp"
#include "core/gettext.hpp"
#include "core/debug.hpp"
#include "tools/time.hpp"
#include "tools/stack.hpp"
#include "tools/random.hpp"
#include "tools/filesystem.hpp"

#include <steam/steam.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h> // STDERR_FILENO

//! The call stack.
tools::CallStack callStack;

#if defined(__GNUC__)
#if not defined(__MINGW32__)
#include <signal.h>

extern "C"
{
    //! Inject the callstack generation each time an exception is launched.
    void __cxa_throw(void* ex, void* info, void (*dest)(void*))
    {
        // Recompute the stack from here, skip this function.
        callStack.refresh(1);

        // We get the original name for the GCC throw
        static void (*const rethrow)(void*,void*,void(*)(void*)) __attribute__ ((noreturn))
            = (void (*)(void*,void*,void(*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");
        rethrow(ex, info, dest);
    }

    //! Inject the callstack generation each time a signal is lauched (SEGFAULT, etc.)
    void handler(int sig)
    {
        throw std::runtime_error(strsignal(sig));
    }
}
#endif
#endif

//! Main function.
int main(int argc, char *argv[])
{
    int returnStatus = EXIT_SUCCESS;

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

    try {
        Application app(args);
        app.run();
        mdebug_core_1("Quitting game safely.")
    }
    catch(std::exception& e) {
        std::stringstream str;
        str << std::endl << "[!] Exception caught: " << e.what() << std::endl;
        str << std::endl << "Here's the call stack when the exception occured:" << std::endl;
        str << callStack.toString();

        // Show message in the console and in a file
        std::cerr << str.str() << std::endl;
        std::cerr << "This message has also been saved to error.log." << std::endl;

        auto errorErrorFileName = "log/error_" + time2string("%Y%m%d-%H%M%S") + ".log";
        std::ofstream errorFile;
        errorFile.open(errorErrorFileName);
        errorFile << str.str();

        returnStatus = EXIT_FAILURE;
    }

    //----- Closing procedures -----//

    // Closing Steam
    if (steamActivated)
        steam::SteamAPI_Shutdown();

    // Free internationalization
    i18n::close();

    return returnStatus;
}

