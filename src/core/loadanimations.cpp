#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

void Application::loadAnimations(const std::initializer_list<std::string>& folders)
{
    // Recursively load all files in resource directory
    for (const auto& folder : folders) {
        uint animationsCount = 0u;

        for (const auto& fileInfo : listFiles("res/" + folder, true)) {
            // Load only animations files
            if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "scml")
                continue;

            s_context.animations.load(fileInfo.fullName);

            ++animationsCount;
        }

        mdebug_core_2("Loaded " << animationsCount << " animations from " << folder << ".");
    }
}

void Application::freeAnimations(const std::initializer_list<std::string>& folders)
{
    for (const auto& folder : folders) {
        s_context.animations.freeMatchingPrefix(folder);

        mdebug_core_2("Freed animations from " << folder << ".");
    }
}

void Application::preloadAnimations()
{
    // Force default
    s_context.animations.load("core/default/default.scml");
    s_context.animations.setDefault("default/default");
}
