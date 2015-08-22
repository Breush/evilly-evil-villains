#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

// TODO We should definitely be loading BCD textures only when needed!

void Application::loadTextures()
{
    uint texturesCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res/tex", true)) {
        // Load only png files
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "png")
            continue;

        s_context.textures.load(fileInfo.fullName);

        ++texturesCount;
    }

    mdebug_core_2("Loaded " << texturesCount << " textures.");

    // Backup
    s_context.textures.setDefault("default");

    // Repeated
    s_context.textures.get("nui/focus").setRepeated(true);
    s_context.textures.get("default").setRepeated(true);
    s_context.textures.get("jumping-toasts/background").setRepeated(true);
    s_context.textures.get("dungeon/sidebar/tab/monsters/cage").setRepeated(true);
    s_context.textures.get("dungeon/inter/outer_wall_west").setRepeated(true);
    s_context.textures.get("dungeon/inter/outer_wall_east").setRepeated(true);

    // Smooth
    s_context.textures.get("jumping-toasts/toast-cut").setSmooth(false);
}
