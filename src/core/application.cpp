#include "core/application.hpp"

#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/selectworld.hpp"
#include "states/game/game.hpp"
#include "states/game/donjondesign.hpp"
#include "states/game/pause.hpp"

#include <SFML/Window/Event.hpp>

#include <string>

const sf::Time Application::s_timePerFrame = sf::seconds(1.f/60.f);
Application::Context Application::s_context(sf::Vector2f(960.f, 540.f), "Evily Evil Villains", sf::Style::Close | sf::Style::Resize);

Application::Application()
    : m_initialState(States::SPLASHCREEN)
    , m_gameTime(0.f)
    , m_running(false)
{
    s_context.window.setKeyRepeatEnabled(false);
    s_context.window.setVerticalSyncEnabled(true);

    loadTextures();
    loadShaders();
    loadFonts();

    registerStates();
    m_stateStack.pushState(m_initialState);
}

void Application::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    m_running = true;
    while (m_running) {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate > s_timePerFrame) {
            timeSinceLastUpdate -= s_timePerFrame;

            processInput();
            update(s_timePerFrame);

            if (m_stateStack.isEmpty()) {
                m_running = false;
            }
        }

        render();
    }

    s_context.window.close();
}

void Application::processInput()
{
    sf::Event event;

    while (s_context.window.pollEvent(event)) {
        m_stateStack.handleEvent(event);

        if (event.type == sf::Event::Closed) {
            m_running = false;
        }
        else if (event.type == sf::Event::Resized) {
            s_context.window.setView(bestView(sf::Vector2f(event.size.width, event.size.height)));
        }
    }
}

void Application::update(sf::Time dt)
{
    // Shaders can be animated
    m_gameTime += dt.asSeconds();

    // TODO Move to a shader code
    s_context.shaders.setParameter(Shaders::MENU_BG, "time", m_gameTime/5.f);
    s_context.shaders.setParameter(Shaders::MENU_NAME, "time", m_gameTime/5.f);

    m_stateStack.update(dt);
}

void Application::render()
{
    s_context.window.clear();
    m_stateStack.draw();
    s_context.window.display();
}

//---------------------//
//----- Resources -----//

void Application::loadTextures()
{
    // NUI
    s_context.textures.load(Textures::NUI_FOCUS, "res/tex/nui/focus.png");
    s_context.textures.setRepeated(Textures::NUI_FOCUS, true);

    // Menu
    s_context.textures.load(Textures::MENU_BG, "res/tex/menu/bg.png");
    s_context.textures.load(Textures::MENU_NAME, "res/tex/menu/name.png");
}

void Application::loadShaders()
{
    // Need to load all shaders before setting parameters
    s_context.shaders.load(Shaders::MENU_BG, "res/shd/menu/bg.vert", "res/shd/menu/bg.frag");
    s_context.shaders.load(Shaders::MENU_NAME, "res/shd/menu/name.vert", sf::Shader::Vertex);
    s_context.shaders.load(Shaders::NUI_HOVER, "res/shd/nui/hover.frag", sf::Shader::Fragment);
    s_context.shaders.load(Shaders::NUI_FOCUS, "res/shd/nui/focus.frag", sf::Shader::Fragment);

    // Parameters
    s_context.shaders.setParameter(Shaders::MENU_BG, "screenSize", s_context.resolution);
    s_context.shaders.setParameter(Shaders::MENU_BG, "texture", sf::Shader::CurrentTexture);

    s_context.shaders.setParameter(Shaders::NUI_HOVER, "textureSize", s_context.resolution);
    s_context.shaders.setParameter(Shaders::NUI_HOVER, "texture", sf::Shader::CurrentTexture);

    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "screenSize", s_context.resolution);
}

void Application::loadFonts()
{
    s_context.fonts.load(Fonts::NUI, "res/font/dream_orphans.ttf");
}

//------------------//
//----- States -----//

void Application::registerStates()
{
    m_stateStack.registerState<SplashScreenState>(States::SPLASHCREEN);

    m_stateStack.registerState<MenuMainState>(States::MENU_MAIN);
    m_stateStack.registerState<MenuSelectWorldState>(States::MENU_SELECTWORLD);

    m_stateStack.registerState<GameGameState>(States::GAME_GAME);
    m_stateStack.registerState<GameDonjonDesignState>(States::GAME_DONJON_DESIGN);
    m_stateStack.registerState<GamePauseState>(States::GAME_PAUSE);
}

//-----------------------------//
//----- Window management -----//

sf::View Application::bestView(const sf::Vector2f& windowSize)
{
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    float screenWidth = windowSize.x / s_context.resolution.x;
    float screenHeight = windowSize.y / s_context.resolution.y;

    if (screenWidth > screenHeight) {
        viewport.width = screenHeight / screenWidth;
        viewport.left = (1.f - viewport.width) / 2.f;
    }
    else if (screenWidth < screenHeight) {
        viewport.height = screenWidth / screenHeight;
        viewport.top = (1.f - viewport.height) / 2.f;
    }

    sf::View view(sf::FloatRect(0, 0, s_context.resolution.x, s_context.resolution.y));
    view.setViewport(viewport);
    return view;
}

