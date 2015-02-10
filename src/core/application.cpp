#include "core/application.hpp"

#include "tools/debug.hpp"
#include "states/quit.hpp"
#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/selectworld.hpp"
#include "states/game/dungeondesign.hpp"
#include "states/game/pause.hpp"

#include <SFML/Window/Event.hpp>
#include <string>

//----------------------------//
//----- Static variables -----//

const sf::Time Application::s_timePerFrame = sf::seconds(1.f/60.f);
Application::Context Application::s_context;

//-------------------//
//----- Context -----//

void Application::Context::init(const sf::Vector2f& iResolution, const std::string& iTitle, const uint32_t& iStyle)
{
    title = iTitle;
    style = iStyle;
    resolution = iResolution;
    effectiveDisplay = iResolution;

    if (window.isOpen())
        window.close();

    window.create(sf::VideoMode(resolution.x, resolution.y), title, style);

    if (!window.isOpen())
        throw std::runtime_error("Cannot initialize window.");

    screenSize = sf::v2f(window.getSize());
}

//-----------------------//
//----- Application -----//

Application::Application()
    //: m_initialState(States::MENU_SELECTWORLD)
    : m_initialState(States::SPLASHSCREEN)
    , m_gameTime(0.f)
    , m_running(false)
{
    s_context.init({1360.f, 768.f}, "Evily Evil Villains", sf::Style::Default);
    s_context.window.setKeyRepeatEnabled(false);
    s_context.window.setVerticalSyncEnabled(true);

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
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    m_running = true;
    while (m_running) {
        // Getting time
        sf::Time dt(clock.restart());
        timeSinceLastUpdate += dt;

        // Update until frame limit is hit
        while (timeSinceLastUpdate > s_timePerFrame) {
            timeSinceLastUpdate -= s_timePerFrame;

            // Game logic core
            processInput();
            update(s_timePerFrame);

            // Quit if no more states
            if (m_stateStack.isEmpty())
                m_running = false;
        }

        // Rendering
        render();
    }

    // Finish closing
    if (s_context.window.isOpen())
        s_context.window.close();
}

void Application::processInput()
{
    sf::Event event;

    while (s_context.window.pollEvent(event)) {

        // Keyboard
        if (event.type == sf::Event::KeyPressed) {
            // Switch visual debug mode
            if (event.key.code == sf::Keyboard::F3) {
                m_visualDebug.switchVisible();
                break;
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
                m_stateStack.clearStates();
                m_stateStack.pushState(States::SPLASHSCREEN);
                break;
            }
#endif
        }

        // Closing window
        if (event.type == sf::Event::Closed) {
            if (!m_stateStack.isStateVisible(States::QUIT))
                m_stateStack.pushState(States::QUIT);
            break;
        }

        // Resizing window
        if (event.type == sf::Event::Resized) {
            s_context.screenSize = sf::Vector2f(event.size.width, event.size.height);
            clearWindowEvents();
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

    // Visual debug
    m_visualDebug.update(dt);

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

void Application::clearWindowEvents()
{
    sf::Event event;
    while (s_context.window.pollEvent(event));
}

void Application::refresh()
{
    s_context.window.setView(bestView());
    m_stateStack.refresh();
    refreshShaders();
}

void Application::switchFullscreenMode()
{
    // Switching fullscreen flag
    s_context.style ^= sf::Style::Fullscreen;
    s_context.init(s_context.resolution, s_context.title, s_context.style);
    refresh();
}

sf::View Application::bestView()
{
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    const auto& screenSize = s_context.screenSize;
    const auto& resolution = s_context.resolution;
    const sf::Vector2f viewRatio = sf::vdiv(screenSize, resolution);

    if (viewRatio.x > viewRatio.y) {
        viewport.width = viewRatio.y / viewRatio.x;
        viewport.left = (1.f - viewport.width) / 2.f;
    }
    else if (viewRatio.x < viewRatio.y) {
        viewport.height = viewRatio.x / viewRatio.y;
        viewport.top = (1.f - viewport.height) / 2.f;
    }

    s_context.effectiveDisplay = sf::Vector2f(screenSize.x * viewport.width, screenSize.y * viewport.height);

    sf::View view(sf::FloatRect(0.f, 0.f, resolution.x, resolution.y));
    view.setViewport(viewport);
    return view;
}

