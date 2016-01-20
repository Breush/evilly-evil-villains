#include "core/application.hpp"

#include "core/debug.hpp"
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
        if (s_context.shaders.fileStored(coupleFile)) continue;

        // If there is a couple of shaders frag/vert to add
        if (fileExists(coupleFile)) {
            // We let vertex file add it
            if (extension != "vert") continue;
            s_context.shaders.load(file, coupleFile);
        }

        // No couple
        else {
            s_context.shaders.load(file, (extension == "frag")? sf::Shader::Fragment : sf::Shader::Vertex);
        }

        ++shadersCount;
    }

    mdebug_core_2("Loaded " << shadersCount << " shaders.");

    // Backup
    s_context.shaders.setDefault("core/default/default");

    refreshShaders();
}

void Application::refreshShaders()
{
    returnif (!sf::Shader::isAvailable());

    const auto& screenSize = s_context.windowInfo.screenSize;
    const auto& resolution = s_context.windowInfo.resolution;
    const auto& effectiveDisplay = s_context.windowInfo.effectiveDisplay;

    // NUI
    s_context.shaders.get("core/nui/hover/hover").setParameter("texture", sf::Shader::CurrentTexture);

    // Splashcreen
    s_context.shaders.get("core/menu/splashscreen/background").setParameter("texture", sf::Shader::CurrentTexture);
    s_context.shaders.get("core/menu/splashscreen/background").setParameter("screenSize", screenSize);
    s_context.shaders.get("core/menu/splashscreen/background").setParameter("resolution", resolution);
    s_context.shaders.get("core/menu/splashscreen/background").setParameter("effectiveDisplay", effectiveDisplay);
}

// TODO Have the shaders working the same way as textures and animations,
// Because these updates do not feel really useful when it's not a menu screen.

void Application::updateShaders(const sf::Time& dt)
{
    returnif (!sf::Shader::isAvailable());

    s_context.shaders.get("core/menu/splashscreen/background").setParameter("time", m_gameTime / 5.f);
    s_context.shaders.get("core/menu/main/logo").setParameter("time", m_gameTime / 5.f);
}

