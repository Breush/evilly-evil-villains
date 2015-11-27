#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

void Application::loadTextures(const std::initializer_list<std::string>& folders)
{
    // Recursively load all files in resource directory
    for (const auto& folder : folders) {
        uint texturesCount = 0u;

        for (const auto& fileInfo : listFiles("res/tex/" + folder, true)) {
            // Load only png files
            if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "png")
                continue;

            s_context.textures.load(fileInfo.fullName).setSmooth(true);

            ++texturesCount;
        }

        mdebug_core_2("Loaded " << texturesCount << " textures from " << folder << ".");
    }
}

void Application::freeTextures(const std::initializer_list<std::string>& folders)
{
    for (const auto& folder : folders) {
        s_context.textures.freeMatchingPrefix(folder);
        mdebug_core_2("Freed textures from " << folder << ".");
    }
}

void Application::preloadTextures()
{
    loadTextures({"global", "menu", "nui"});

    // Force default
    s_context.textures.load("res/tex/default.png");
    s_context.textures.get("default").setRepeated(true);
    s_context.textures.setDefault("default");

    // And NUI
    s_context.textures.get("nui/focus").setRepeated(true);
}
