#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/vector.hpp"
#include "states/identifiers.hpp"

#include <SFML/Window/Event.hpp>
#include <string>

//----------------------------//
//----- Static variables -----//

Application::Context Application::s_context;

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
    debug_core_1(const auto& settings = window.getSettings());
    mdebug_core_1("OpenGL version used: " << settings.majorVersion << "." << settings.minorVersion);
    mdebug_core_1("Depth bits: " << settings.depthBits << " | Stencil bits: " << settings.stencilBits);
    mdebug_core_1("Antialiasing level: " << settings.antialiasingLevel);
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

    // Context - create window
    s_context.windowInfo.title = "Evilly Evil Villains";
    s_context.windowInfo.style = sf::Style::Default;
    if (s_context.display.window.fullscreen) switchFullscreenMode();
    else s_context.recreateWindow();

    // Load all on start
    // TODO Well, do not ALL on start,
    // or, at least, do it during some splash/loading-screen
    loadTextures();
    loadShaders();
    loadFonts();
    loadMusics();
    loadSounds();
    loadAnimations();
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
        sf::Time dt(clock.restart());
        lag += dt;

        // Update until frame limit is hit
        while (lag >= m_timePerFrame) {
            lag -= m_timePerFrame;

            // Game logic core
            processInput();
            m_visualDebug.update(m_timePerFrame);
            update(m_gameTimeFactor * m_timePerFrame);

            // Quit if no more states
            if (m_stateStack.isEmpty())
                m_running = false;
        }

        // Rendering
        // TODO render(lag/m_timePerFrame);
        // So that physics can interpolate the effective display
        // See http://gameprogrammingpatterns.com/game-loop.html
        render();
    }

    // Finish closing
    if (s_context.window.isOpen())
        s_context.window.close();
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
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
            clearWindowEvents(event, sf::Event::Resized);
            s_context.windowInfo.screenSize = sf::Vector2f(event.size.width, event.size.height);
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

    // Game logic
    updateShaders(dt);
    updateAnimations(dt);
    m_stateStack.update(dt);
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
    refreshSounds();
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
