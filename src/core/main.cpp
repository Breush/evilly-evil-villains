#include "core/application.hpp"
#include "core/gettext.hpp"
#include "core/debug.hpp"
#include "tools/stack.hpp"
#include "tools/random.hpp"

#include <steam/steam.hpp>

#include <stdexcept>
#include <iostream>

//! The call stack.
tools::CallStack callStack;

#if defined(__GNUC__)
#if not defined(__MINGW32__)
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
}
#endif
#endif

//! Main function.
int main(int argc, char *argv[])
{
    Application app;
    int returnStatus = EXIT_SUCCESS;

    // Disable synchronisation
    // Note: Remove the safety of using printf/scanf (so don't use them)
    // but this makes the streams faster.
    // std::ios_base::sync_with_stdio(false);
    // Note: In fact, as we do not control all libraries,
    // keeping in sync is important to avoid interleaved characters.
    std::ios_base::sync_with_stdio(true);

    // Forcing wide/narrow encodings.
    std::wcout << L"";
    std::cerr << "";

    // Handle arguments
    for (int i = 1; i < argc; ++i)
        mdebug_core_1("Argument " << i << " : " << argv[i]);

    // Initialize randomness
    alea::s_generator.seed(time(nullptr));

    // Start Steam
    bool steamActivated = false;
    // No need to try: there nothing to activate unless we get an appID,
    // which supposed to have been greenlit.
    // steamActivated = steam::SteamAPI_Init();
    mdebug_core_2("Steam interfacing: " << (steamActivated? "Enabled" : "Disabled"));

    // Running application
    try {
        app.run();
    }
    catch(std::exception& e) {
        std::cerr << std::endl << "[!] Exception caught: " << e.what() << std::endl;
        std::cerr << std::endl << "Here's the call stack when the exception occured:" << std::endl;
        std::cerr << callStack.toString();
        returnStatus = EXIT_FAILURE;
    }

    // Closing Steam
    if (steamActivated)
        steam::SteamAPI_Shutdown();

    // Free internationalization
    i18n::close();

    return returnStatus;
}

