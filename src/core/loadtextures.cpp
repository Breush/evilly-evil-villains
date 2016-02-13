#include "core/application.hpp"

#include "core/debug.hpp"
#include "context/context.hpp"
#include "tools/filesystem.hpp"

void Application::loadTextures(const std::initializer_list<std::string>& folders)
{
    // Recursively load all files in resource directory
    for (const auto& folder : folders) {
        uint texturesCount = 0u;

        for (const auto& fileInfo : listFiles("res/" + folder, true)) {
            // Load only png files
            if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "png")
                continue;

            context::context.textures.load(fileInfo.fullName).setSmooth(true);

            ++texturesCount;
        }

        mdebug_core_2("Loaded " << texturesCount << " textures from " << folder << ".");
    }
}

void Application::freeTextures(const std::initializer_list<std::string>& folders)
{
    for (const auto& folder : folders) {
        context::context.textures.freeMatchingPrefix(folder);
        mdebug_core_2("Freed textures from " << folder << ".");
    }
}

void Application::preloadTextures()
{
    loadTextures({"core/default", "core/cursor", "core/global", "core/ltbl", "core/menu/main", "core/menu/loading", "core/nui", "core/posteffects"});

    // Force default
    context::context.textures.get("core/default/default").setRepeated(true);
    context::context.textures.setDefault("core/default/default");

    // Post-effects
    context::context.textures.get("core/posteffects/floomzig/floom").setRepeated(true);

    // NUI
    context::context.textures.get("core/nui/focus/background").setRepeated(true);
}
