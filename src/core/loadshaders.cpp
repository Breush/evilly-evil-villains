#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

void Application::loadShaders()
{
    // TODO Yeah, sure... but... what do we do about that?
    returnif (!sf::Shader::isAvailable());

    uint shadersCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res/shd", true)) {
        // Load only shader files
        const auto& file = fileInfo.fullName;
        auto extension = fileExtension(file);
        if (fileInfo.isDirectory || (extension != "frag" && extension != "vert"))
            continue;

        // Get ID
        std::string fileNoExtension = file.substr(0u, file.find_last_of("."));
        std::string shaderID = fileNoExtension.substr(fileNoExtension.find_first_of("/") + 1u);
        shaderID = shaderID.substr(shaderID.find_first_of("/") + 1u);

        // If we did not already add a couple
        if (s_context.shaders.exists(shaderID)) continue;

        // If there is a couple of shaders frag/vert to add
        std::string coupleFile = fileNoExtension + "." + ((extension == "frag")? "vert" : "frag");
        if (fileExists(coupleFile)) {
            // We let vertex file add it
            if (extension != "vert") continue;
            s_context.shaders.load(shaderID, file, coupleFile);
        }

        // No couple
        else {
            s_context.shaders.load(shaderID, file, (extension == "frag")? sf::Shader::Fragment : sf::Shader::Vertex);
        }

        ++shadersCount;
    }

    mdebug_core_1("Loaded " << shadersCount << " shaders.");

    refreshShaders();
}

void Application::refreshShaders()
{
    returnif (!sf::Shader::isAvailable());

    const auto& screenSize = s_context.windowInfo.screenSize;
    const auto& resolution = s_context.windowInfo.resolution;
    const auto& effectiveDisplay = s_context.windowInfo.effectiveDisplay;

    // NUI
    s_context.shaders.get("nui/hover").setParameter("texture", sf::Shader::CurrentTexture);
    s_context.shaders.get("nui/focus").setParameter("screenSize", screenSize);
    s_context.shaders.get("nui/focus").setParameter("resolution", resolution);
    s_context.shaders.get("nui/focus").setParameter("effectiveDisplay", effectiveDisplay);

    // Splashcreen
    s_context.shaders.get("menu/splashscreen").setParameter("texture", sf::Shader::CurrentTexture);
    s_context.shaders.get("menu/splashscreen").setParameter("screenSize", screenSize);
    s_context.shaders.get("menu/splashscreen").setParameter("resolution", resolution);
    s_context.shaders.get("menu/splashscreen").setParameter("effectiveDisplay", effectiveDisplay);

    // Menu
    s_context.shaders.get("menu/background").setParameter("resolution", resolution);
}

void Application::updateShaders(const sf::Time& dt)
{
    s_context.shaders.get("menu/splashscreen").setParameter("time", m_gameTime / 5.f);
    s_context.shaders.get("menu/background").setParameter("time", m_gameTime / 25.f);
    s_context.shaders.get("menu/name").setParameter("time", m_gameTime / 5.f);
}

