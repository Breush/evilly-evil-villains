#include "core/application.hpp"

#include "core/debug.hpp"
#include "core/gettext.hpp"
#include "states/identifiers.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

#include <SFML/Window/Event.hpp>
#include <string>

//----------------------------//
//----- Static variables -----//

Application::Context Application::s_context;
VisualDebug Application::s_visualDebug;
bool Application::s_needRefresh = false;
bool Application::s_paused = false;

//-------------------//
//----- Context -----//

void Application::Context::recreateWindow()
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
    if (icon.loadFromFile("res/tex/global/icon.png"))
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

//-----------------//
//----- Extra -----//

//! Cleans files that are created and can interfere in debug mode.
void cleanExtraFiles()
{
    std::remove("saves/villains_saved.xml");
    std::remove("saves/worlds_saved.xml");

    for (const auto& fileInfo : listFiles("saves/", true))
        if (fileInfo.name == "dungeon_saved.xml")
            std::remove(fileInfo.fullName.c_str());
}

//-----------------------//
//----- Application -----//

Application::Application()
    : m_initialState(StateID::SPLASHSCREEN)
{
    // Context
    s_context.windowInfo.title = "Evilly Evil Villains";
    i18n::initLanguagesList();
    refreshFromConfig();

    // Load all on start (except textures)
    preloadTextures();
    loadShaders();
    loadFonts();
    loadMusics();
    loadSounds();
    preloadAnimations();
    loadStates();

    // All is ready, go for it
    m_stateStack.pushState(m_initialState);
    s_visualDebug.init();
    m_cursor.init();

#if DEBUG_GLOBAL > 0
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        mdebug_core_1("Cleaning saved files from last session. Disable this by pressing LControl during application start.");
        cleanExtraFiles();
    }
#endif
}

void Application::run()
{
    sf::Clock clock;
    sf::Clock debugClock;
    sf::Time lag(sf::Time::Zero);

    m_running = true;
    while (m_running) {
        // Getting time
        lag += clock.restart();

        // Inputs
        processInput();
        synchronizeMouse();

        // Update until frame limit is hit
        while (lag >= m_updateTime) {
            lag -= m_updateTime;

            // Game logic core
            debugClock.restart();
            update(m_updateTime);
            s_visualDebug.setLogicTickTime(debugClock.getElapsedTime());

            // Quit if no more states
            if (m_stateStack.isEmpty())
                m_running = false;
        }

        // Rendering
        // We might be doing render(lag/m_updateTime);
        // So that physics can interpolate the effective display
        // See http://gameprogrammingpatterns.com/game-loop.html
        //  However, this seems a bit too much,
        // and SFML would not let us do that easily.
        render();
    }

    // Finish closing
    if (s_context.window.isOpen())
        s_context.window.close();
}

void Application::setPaused(bool paused)
{
    returnif (s_paused == paused);

    s_paused = paused;
    s_context.sounds.setPaused(s_paused);
    s_context.musics.setPaused(s_paused);
}

//-------------------------------//
//----- Logic and rendering -----//

void Application::processInput()
{
    sf::Event event;

    while (s_context.window.pollEvent(event)) {
        // Discard mouse moved events as we create them each frame anyway
        if (event.type == sf::Event::MouseMoved)
            continue;

        // Keyboard
        if (event.type == sf::Event::KeyPressed) {
            // Switch visual debug mode
            if (event.key.code == sf::Keyboard::F3) {
                s_visualDebug.switchVisible();
                continue;
            }

            // Switch fullscreen mode
            if (event.key.code == sf::Keyboard::F11) {
                switchFullscreenMode();
                clearWindowEvents();
                break;
            }

            // Show terminal
            if (event.key.shift && event.key.code == sf::Keyboard::F2) {
                if (!m_stateStack.isStateVisible(StateID::TERMINAL))
                    m_stateStack.pushState(StateID::TERMINAL);
                continue;
            }

#if DEBUG_GLOBAL > 0
            // Hard reset on debug mode
            if (event.key.code == sf::Keyboard::BackSlash) {
                m_gameTime = 0.f;
                m_stateStack.clearStates();
                m_stateStack.pushState(StateID::SPLASHSCREEN);
                continue;
            }
#endif
        }

        // Closing window
        if (event.type == sf::Event::Closed) {
            if (!m_stateStack.isStateVisible(StateID::QUIT))
                m_stateStack.pushState(StateID::QUIT);
            clearWindowEvents();
            break;
        }

        // Resizing window (grab only the last of these events)
        if (event.type == sf::Event::Resized) {
            clearWindowEvents(event, sf::Event::Resized);

            // Security over the new size, a 0 dimension implies OpenGL bugs
            sf::Vector2u newSize(event.size.width, event.size.height);
            if (newSize.x < 10u || newSize.y < 10u) {
                    if (newSize.x < 10u) newSize.x = 10u;
                    if (newSize.y < 10u) newSize.y = 10u;
                    s_context.window.setSize(newSize);
                    continue;
            }

            s_context.windowInfo.screenSize = sf::v2f(newSize);
            refreshWindow();
            continue;
        }

        m_stateStack.handleEvent(event);
        m_cursor.handleEvent(event);
    }

}

void Application::synchronizeMouse()
{
    const auto& mousePosition = sf::Mouse::getPosition(s_context.window);

    sf::Event mouseMovedEvent;
    mouseMovedEvent.type = sf::Event::MouseMoved;
    mouseMovedEvent.mouseMove.x = mousePosition.x;
    mouseMovedEvent.mouseMove.y = mousePosition.y;
    m_stateStack.handleEvent(mouseMovedEvent);
    m_cursor.handleEvent(mouseMovedEvent);
}

void Application::update(const sf::Time& dt)
{
    // Refresh asked
    if (s_needRefresh) {
        refreshNUI();
        refreshWindow();
        s_needRefresh = false;
    }

    // Shaders can be animated
    m_gameTime += dt.asSeconds();

    // Overall elements
    s_visualDebug.update(dt);
    m_cursor.update(dt);

    // Game logic
    s_context.commander.update(dt);
    m_stateStack.update(dt);

    // Update the visual effects
    // FIXME Sounds and shaders are not affected by States's timefactor
    if (!s_paused) {
        updateSounds(dt);
        updateShaders(dt);
    }
}

void Application::render()
{
    s_context.window.clear();
    s_context.window.draw(m_stateStack);
    s_context.window.draw(s_visualDebug);
    s_context.window.draw(m_cursor);
    s_context.window.display();
}

//-----------------------------//
//----- Window management -----//

void Application::refreshFromConfig(bool refreshWindow, bool refreshAudio)
{
    // Language
    i18n::init(toString(s_context.display.global.language));

    // Window
    if (refreshWindow) {
        s_context.windowInfo.style = (s_context.display.window.fullscreen)? sf::Style::Fullscreen : sf::Style::Default;
        s_context.recreateWindow();
    }

    // Audio
    if (refreshAudio) {
        refreshSounds();
        refreshMusics();
    }

    // Marking the need for future refresh of NUI/Window
    s_needRefresh = true;
}

void Application::refreshNUI()
{
    s_context.nuiGuides.recompute(s_context.display.nui);

    m_stateStack.refreshNUI(s_context.nuiGuides);
    s_visualDebug.refreshNUI(s_context.nuiGuides);
}

void Application::refreshWindow()
{
    s_context.windowInfo.recompute();

    // Refresh all views
    m_stateStack.refreshWindow(s_context.windowInfo);
    s_visualDebug.refreshWindow(s_context.windowInfo);
    m_cursor.refreshWindow(s_context.windowInfo);

    // Refresh shaders
    refreshShaders();
}

void Application::clearWindowEvents()
{
    sf::Event polledEvent;
    while (s_context.window.pollEvent(polledEvent));
}

void Application::clearWindowEvents(sf::Event& event, sf::Event::EventType type)
{
    sf::Event polledEvent;
    while (s_context.window.pollEvent(polledEvent))
        if (polledEvent.type == type)
            event = polledEvent;
}

void Application::switchFullscreenMode()
{
    // Switching fullscreen flag
    s_context.display.window.fullscreen = !s_context.display.window.fullscreen;
    refreshFromConfig();
    refreshWindow();
}
