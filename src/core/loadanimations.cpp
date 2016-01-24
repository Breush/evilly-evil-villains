#include "core/application.hpp"

#include "core/debug.hpp"
#include "context/context.hpp"
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

            context::context.animations.load(fileInfo.fullName);

            ++animationsCount;
        }

        mdebug_core_2("Loaded " << animationsCount << " animations from " << folder << ".");
    }
}

void Application::freeAnimations(const std::initializer_list<std::string>& folders)
{
    for (const auto& folder : folders) {
        context::context.animations.freeMatchingPrefix(folder);

        mdebug_core_2("Freed animations from " << folder << ".");
    }
}

void Application::preloadAnimations()
{
    // Force default
    context::context.animations.load("core/default/default.scml");
    context::context.animations.setDefault("default/default");
}
