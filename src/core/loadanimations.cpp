#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

void Application::loadAnimations()
{
    uint animationsCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res/scml", true)) {
        // Load only animations files
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "scml")
            continue;

        s_context.animations.load(fileInfo.fullName);

        ++animationsCount;
    }

    mdebug_core_2("Loaded " << animationsCount << " animations.");
}

void Application::updateAnimations(const sf::Time& dt)
{
    s_context.animations.update(dt);
}
