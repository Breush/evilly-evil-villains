#include "core/application.hpp"

#include "core/debug.hpp"
#include "context/context.hpp"
#include "tools/filesystem.hpp"

void Application::loadFonts()
{
    uint fontsCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res/core/global/fonts", true)) {
        // Load only font files
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "ttf")
            continue;

        context::context.fonts.load(fileInfo.fullName);

        ++fontsCount;
    }

    mdebug_core_2("Loaded " << fontsCount << " fonts.");
}
