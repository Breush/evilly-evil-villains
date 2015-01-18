#include "core/application.hpp"

#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/selectworld.hpp"
#include "states/game/game.hpp"
#include "states/game/donjondesign.hpp"
#include "states/game/pause.hpp"

#include <SFML/Window/Event.hpp>

#include <string>

const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application()
    : m_window(sf::VideoMode(960, 540), "Evily Evil Villains", sf::Style::Close | sf::Style::Resize)
    , m_textures()
    , m_fonts()
    , m_stateStack(State::Context(m_window, m_textures, m_shaders,
                                  m_fonts, m_music, m_sounds))
    , m_initialState(States::SPLASHCREEN)
    , m_gameTime(0.f)
    , m_running(false)
{
    m_window.setKeyRepeatEnabled(false);
    m_window.setVerticalSyncEnabled(true);

    /*sf::View view(m_window.getView());
    view.setViewport(sf::FloatRect(0.25f, 0.25f, 0.5f, 0.5f));
    m_window.setView(view);*/

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

        while(timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;

            processInput();
            update(TimePerFrame);

            if(m_stateStack.isEmpty()) {
                m_running = false;
            }
        }

        render();
    }

    m_window.close();
}

void Application::processInput()
{
    sf::Event event;

    while (m_window.pollEvent(event)) {
        m_stateStack.handleEvent(event);

        if (event.type == sf::Event::Closed) {
            m_running = false;
        }
    }
}

void Application::update(sf::Time dt)
{
    // Shaders can be animated
    m_gameTime += dt.asSeconds();
    // TODO Déplacer dans le code du shader
    m_shaders.setParameter(Shaders::MENU_BG, "time", m_gameTime/5.f);
    m_shaders.setParameter(Shaders::MENU_NAME, "time", m_gameTime/5.f);

    m_stateStack.update(dt);
}

void Application::render()
{
    m_window.clear();
    m_stateStack.draw();
    m_window.display();
}

//---------------------//
//----- Resources -----//

void Application::loadTextures()
{
    // NUI
    m_textures.load(Textures::NUI_FOCUS, "res/tex/nui/focus.png");
    m_textures.setRepeated(Textures::NUI_FOCUS, true);

    // Menu
    m_textures.load(Textures::MENU_BG, "res/tex/menu/bg.png");
    m_textures.load(Textures::MENU_NAME, "res/tex/menu/name.png");
}

void Application::loadShaders()
{
    // Need to load all shaders before setting parameters
    m_shaders.load(Shaders::MENU_BG, "res/shd/menu/bg.vert", "res/shd/menu/bg.frag");
    m_shaders.load(Shaders::MENU_NAME, "res/shd/menu/name.vert", sf::Shader::Vertex);
    m_shaders.load(Shaders::NUI_HOVER, "res/shd/nui/hover.frag", sf::Shader::Fragment);
    m_shaders.load(Shaders::NUI_FOCUS, "res/shd/nui/focus.frag", sf::Shader::Fragment);

    // Parameters
    m_shaders.setParameter(Shaders::MENU_BG, "screenSize", m_window.getView().getSize());
    m_shaders.setParameter(Shaders::MENU_BG, "texture", sf::Shader::CurrentTexture);

    m_shaders.setParameter(Shaders::NUI_HOVER, "textureSize", m_window.getView().getSize());
    m_shaders.setParameter(Shaders::NUI_HOVER, "texture", sf::Shader::CurrentTexture);

    m_shaders.setParameter(Shaders::NUI_FOCUS, "screenSize", m_window.getView().getSize());
}

void Application::loadFonts()
{
    m_fonts.load(Fonts::NUI, "res/font/dream_orphans.ttf");
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

