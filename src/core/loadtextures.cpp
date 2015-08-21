#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

// TODO Make all textures and sounds to shaders principle? (no res/xxx/ nor extension)

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

    mdebug_core_1("Loaded " << texturesCount << " textures.");

    // Repeated
    s_context.textures.get("res/tex/nui/focus.png").setRepeated(true);
    s_context.textures.get("res/tex/default.png").setRepeated(true);
    s_context.textures.get("res/tex/jumping-toasts/background.png").setRepeated(true);
    s_context.textures.get("res/tex/dungeon/sidebar/tab/monsters/cage.png").setRepeated(true);
    s_context.textures.get("res/tex/dungeon/inter/outer_wall_west.png").setRepeated(true);
    s_context.textures.get("res/tex/dungeon/inter/outer_wall_east.png").setRepeated(true);

    // Smooth
    s_context.textures.get("res/tex/jumping-toasts/toast-cut.png").setSmooth(false);
}
