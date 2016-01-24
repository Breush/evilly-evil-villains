#include "core/application.hpp"

#include "core/debug.hpp"
#include "context/context.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

void Application::loadShaders()
{
    returnif (!sf::Shader::isAvailable());

    uint shadersCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res", true)) {
        // Load only shader files
        const auto& file = fileInfo.fullName;
        auto extension = fileExtension(file);
        if (fileInfo.isDirectory || (extension != "frag" && extension != "vert"))
            continue;

        // Is there a corresponding vert/frag file?
        std::string coupleFile = file.substr(0u, file.find_last_of("."));
        coupleFile = coupleFile + ((extension == "frag")? ".vert" : ".frag");

        // If we did not already add a couple
        if (context::context.shaders.fileStored(coupleFile)) continue;

        // If there is a couple of shaders frag/vert to add
        if (fileExists(coupleFile)) {
            // We let vertex file add it
            if (extension != "vert") continue;
            context::context.shaders.load(file, coupleFile);
        }

        // No couple
        else {
            context::context.shaders.load(file, (extension == "frag")? sf::Shader::Fragment : sf::Shader::Vertex);
        }

        ++shadersCount;
    }

    mdebug_core_2("Loaded " << shadersCount << " shaders.");

    // Backup
    context::context.shaders.setDefault("core/default/default");

    refreshShaders();
}

void Application::refreshShaders()
{
    returnif (!sf::Shader::isAvailable());

    const auto& screenSize = context::context.windowInfo.screenSize;
    const auto& resolution = context::context.windowInfo.resolution;
    const auto& effectiveDisplay = context::context.windowInfo.effectiveDisplay;

    // NUI
    context::context.shaders.get("core/nui/hover/hover").setParameter("texture", sf::Shader::CurrentTexture);

    // Splashcreen
    context::context.shaders.get("core/menu/splashscreen/background").setParameter("texture", sf::Shader::CurrentTexture);
    context::context.shaders.get("core/menu/splashscreen/background").setParameter("screenSize", screenSize);
    context::context.shaders.get("core/menu/splashscreen/background").setParameter("resolution", resolution);
    context::context.shaders.get("core/menu/splashscreen/background").setParameter("effectiveDisplay", effectiveDisplay);
}

// TODO Have the shaders working the same way as textures and animations,
// Because these updates do not feel really useful when it's not a menu screen.

void Application::updateShaders(const sf::Time& dt)
{
    returnif (!sf::Shader::isAvailable());

    context::context.shaders.get("core/menu/splashscreen/background").setParameter("time", m_gameTime / 5.f);
    context::context.shaders.get("core/menu/main/logo").setParameter("time", m_gameTime / 5.f);
}

