#include "core/application.hpp"

#include "core/debug.hpp"
#include "core/gettext.hpp"
#include "states/identifiers.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"

#include <SFML/Window/Event.hpp>
#include <string>

//----------------------------//
//----- Static variables -----//

Application::Context Application::s_context;
bool Application::s_paused = false;

//-------------------//
//----- Context -----//

void Application::Context::recreateWindow()
{
    const auto& title = windowInfo.title;
    const auto& style = windowInfo.style;
    const auto& resolution = display.window.resolution;
    const auto& antialiasingLevel = display.window.antialiasingLevel;

    mdebug_core_1("Recreating window with resolution " << resolution);

    if (window.isOpen())
        window.close();

    sf::ContextSettings contextSettings;
    contextSettings.antialiasingLevel = antialiasingLevel;
    window.create(sf::VideoMode(resolution.x, resolution.y), title, style, contextSettings);

    if (!window.isOpen())
        throw std::runtime_error("Cannot initialize window.");

    // Window parameters
    sf::Image icon;
    if (icon.loadFromFile("res/tex/global/icon.png"))
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setVerticalSyncEnabled(true);

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
}

//-----------------------//
//----- Application -----//

Application::Application()
    : m_initialState(StateID::SPLASHSCREEN)
{
#if DEBUG_GLOBAL > 0
    cleanExtraFiles();
#endif

    // Context
    s_context.windowInfo.title = "Evilly Evil Villains";
    refreshFromConfig();

    // Initialize language
    internationalization::init(toString(s_context.display.global.language));

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
    m_visualDebug.init();

    // Full refresh on start
    refreshWindow();
    refreshNUI();
}

void Application::run()
{
    sf::Clock clock;
    sf::Time lag(sf::Time::Zero);

    m_running = true;
    while (m_running) {
        // Getting time
        lag += clock.restart();

        // Inputs
        processInput();

        // Update until frame limit is hit
        while (lag >= m_updateTime) {
            lag -= m_updateTime;

            // Game logic core
            update(m_updateTime);

            // Quit if no more states
            if (m_stateStack.isEmpty())
                m_running = false;
        }

        // Rendering
        // TODO render(lag/m_updateTime);
        // So that physics can interpolate the effective display
        // See http://gameprogrammingpatterns.com/game-loop.html
        render();
    }

    // Finish closing
    if (s_context.window.isOpen())
        s_context.window.close();
}

void Application::setPaused(bool paused)
{
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
        // Keyboard
        if (event.type == sf::Event::KeyPressed) {
            // Switch visual debug mode
            if (event.key.code == sf::Keyboard::F3) {
                m_visualDebug.switchVisible();
                continue;
            }

            // Decelerate time
            if (event.key.code == sf::Keyboard::F8) {
                m_gameTimeFactor /= 2.f;
                m_visualDebug.setDisplayedTimeFactor(m_gameTimeFactor);
                continue;
            }

            // Accelerate time
            if (event.key.code == sf::Keyboard::F9) {
                m_gameTimeFactor *= 2.f;
                m_visualDebug.setDisplayedTimeFactor(m_gameTimeFactor);
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
                m_gameTimeFactor = 1.f;
                m_stateStack.clearStates();
                m_stateStack.pushState(StateID::SPLASHSCREEN);
                continue;
            }

            // NUI quick size change
            if (event.key.control) {
                if (event.key.code == sf::Keyboard::Numpad8) {
                    s_context.display.nui.size += 1u;
                    std::cerr << "NUI size: " << s_context.display.nui.size << std::endl;
                    refreshNUI();
                    continue;
                }

                if (event.key.code == sf::Keyboard::Numpad2) {
                    if (s_context.display.nui.size != 0u) s_context.display.nui.size -= 1u;
                    std::cerr << "NUI size: " << s_context.display.nui.size << std::endl;
                    refreshNUI();
                    continue;
                }

                if (event.key.code == sf::Keyboard::Numpad7) {
                    std::cerr << "Forcing refreshNUI()" << std::endl;
                    refreshNUI();
                    continue;
                }

                if (event.key.code == sf::Keyboard::Numpad9) {
                    std::cerr << "Forcing refreshWindow()" << std::endl;
                    refreshWindow();
                    continue;
                }
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
            // Security over the new size, a 0 dimension implies OpenGL bugs
            sf::Vector2u newSize(event.size.width, event.size.height);
            if (newSize.x < 10u || newSize.y < 10u) {
                    if (newSize.x < 10u) newSize.x = 10u;
                    if (newSize.y < 10u) newSize.y = 10u;
                    s_context.window.setSize(newSize);
            }

            clearWindowEvents(event, sf::Event::Resized);
            s_context.windowInfo.screenSize = sf::v2f(newSize);
            s_context.windowInfo.recompute();
            refreshWindow();
            break;
        }

        m_stateStack.handleEvent(event);
    }
}

void Application::update(const sf::Time& dt)
{
    // Shaders can be animated
    m_gameTime += dt.asSeconds();

    // Overall elements
    m_visualDebug.update(dt);

    // Game logic
    const sf::Time dtGame = dt * m_gameTimeFactor;
    s_context.commander.update(dtGame);
    m_stateStack.update(dtGame);

    // Update the visual effects
    if (!s_paused) {
        updateSounds(dtGame);
        updateAnimations(dtGame);
        updateShaders(dtGame);
    }
}

void Application::render()
{
    s_context.window.clear();
    s_context.window.draw(m_stateStack);
    s_context.window.draw(m_visualDebug);
    s_context.window.display();
}

//-----------------------------//
//----- Window management -----//

void Application::refreshFromConfig()
{
    // Graphics
    s_context.windowInfo.style = sf::Style::Default;
    if (s_context.display.window.fullscreen) s_context.windowInfo.style |= sf::Style::Fullscreen;
    s_context.recreateWindow();

    // Audio
    refreshSounds();
    refreshMusics();
}

void Application::refreshNUI()
{
    s_context.nuiGuides.recompute(s_context.display.nui);

    m_stateStack.refreshNUI(s_context.nuiGuides);
    m_visualDebug.refreshNUI(s_context.nuiGuides);
}

void Application::refreshWindow()
{
    // Refresh all views
    m_stateStack.refreshWindow(s_context.windowInfo);
    m_visualDebug.refreshWindow(s_context.windowInfo);

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
    s_context.windowInfo.style ^= sf::Style::Fullscreen;
    s_context.recreateWindow();
    refreshWindow();
}
