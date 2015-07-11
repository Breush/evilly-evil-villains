#include "core/application.hpp"

#include "tools/debug.hpp"
#include "tools/vector.hpp"
#include "config/display.hpp"
#include "states/identifiers.hpp"

#include <SFML/Window/Event.hpp>
#include <string>

//----------------------------//
//----- Static variables -----//

Application::Context Application::s_context;

//-------------------//
//----- Context -----//

void Application::Context::init(const sf::Vector2f& iResolution, const std::string& iTitle, const uint32_t& iStyle)
{
    title = iTitle;
    style = iStyle;
    resolution = iResolution;
    effectiveDisplay = resolution;

    if (window.isOpen())
        window.close();

    window.create(sf::VideoMode(resolution.x, resolution.y), title, style);

    if (!window.isOpen())
        throw std::runtime_error("Cannot initialize window.");

    window.setVerticalSyncEnabled(true);
    screenSize = sf::v2f(window.getSize());
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

    s_context.init(s_context.display.resolution, "Evilly Evil Villains", sf::Style::Default);
    if (s_context.display.fullscreen) switchFullscreenMode();
    else refresh();

    loadTextures();
    loadShaders();
    loadFonts();
    loadMusics();
    loadSounds();
    loadAnimations();
    loadStates();

    m_stateStack.pushState(m_initialState);
    m_visualDebug.init();
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

            // TODO Remove when config interface exists: NUI quick size change
            if (event.key.code == sf::Keyboard::Numpad8) {
                s_context.display.nui += 1;
                std::cerr << "NUI size: " << s_context.display.nui << std::endl;
                refreshDisplay();
                continue;
            }

            if (event.key.code == sf::Keyboard::Numpad2) {
                s_context.display.nui -= 1;
                std::cerr << "NUI size: " << s_context.display.nui << std::endl;
                refreshDisplay();
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

        // Resizing window
        if (event.type == sf::Event::Resized) {
            // Grab only the last resize event
            clearWindowEvents(event, sf::Event::Resized);
            s_context.screenSize = sf::Vector2f(event.size.width, event.size.height);
            refresh();
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

    m_stateStack.draw();
    m_visualDebug.draw();

    s_context.window.display();
}

//-----------------------------//
//----- Window management -----//

void Application::refreshDisplay()
{
    const auto& screenSize = s_context.screenSize;
    const auto& resolution = s_context.resolution;

    s_context.viewport = {0.f, 0.f, 1.f, 1.f};
    const auto viewRatio = screenSize / resolution;

    if (viewRatio.x > viewRatio.y) {
        s_context.viewport.width = viewRatio.y / viewRatio.x;
        s_context.viewport.left = (1.f - s_context.viewport.width) / 2.f;
    }
    else if (viewRatio.x < viewRatio.y) {
        s_context.viewport.height = viewRatio.x / viewRatio.y;
        s_context.viewport.top = (1.f - s_context.viewport.height) / 2.f;
    }

    s_context.effectiveDisplay = {screenSize.x * s_context.viewport.width, screenSize.y * s_context.viewport.height};

    // Refresh all views
    m_stateStack.refreshDisplay();
    m_visualDebug.refreshDisplay();
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
    s_context.style ^= sf::Style::Fullscreen;
    s_context.init(s_context.resolution, s_context.title, s_context.style);
    refresh();
}

void Application::refresh()
{
    refreshDisplay();
    refreshShaders();
}

