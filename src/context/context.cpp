#include "context/context.hpp"

#include "core/debug.hpp"

using namespace context;

//----------------------------//
//----- Global variables -----//

Context context::context;

//-------------------//
//----- Context -----//

void Context::recreateWindow()
{
    const auto& title = windowInfo.title;
    const auto& style = windowInfo.style;
    const auto& vsync = display.window.vsync;
    const auto& resolution = display.window.resolution;
    const auto& antialiasingLevel = display.window.antialiasingLevel;

    mdebug_core_1("Recreating window with resolution " << resolution);

    if (window.isOpen())
        window.close();

    sf::ContextSettings contextSettings;
    contextSettings.antialiasingLevel = antialiasingLevel;
    sf::VideoMode videoMode{static_cast<uint>(resolution.x), static_cast<uint>(resolution.y), sf::VideoMode::getDesktopMode().bitsPerPixel};
    window.create(videoMode, title, style, contextSettings);

    if (!window.isOpen())
        throw std::runtime_error("Cannot initialize window.");

    // Window parameters
    sf::Image icon;
    if (icon.loadFromFile("res/core/global/icon/icon.png"))
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setVerticalSyncEnabled(vsync);
    window.setMouseCursorVisible(false);
    window.setFramerateLimit(120); // TODO Put that value in the config

    // Window info
    windowInfo.screenSize = sf::v2f(window.getSize());
    windowInfo.recompute();

    // Global log
    debug_core_2(const auto& settings = window.getSettings());
    mdebug_core_2("OpenGL version used: " << settings.majorVersion << "." << settings.minorVersion);
    mdebug_core_2("Depth bits: " << settings.depthBits << " | Stencil bits: " << settings.stencilBits);
    mdebug_core_2("Antialiasing level: " << settings.antialiasingLevel);
}
