#include "core/application.hpp"

#include "tools/debug.hpp"
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
        std::cerr << "Cannot initialize window" << std::endl;

    screenSize = sf::v2f(window.getSize());
}

//-----------------------//
//----- Application -----//

Application::Application()
    //: m_initialState(States::GAME_DUNGEON_DESIGN)
    : m_initialState(States::SPLASHSCREEN)
    , m_gameTime(0.f)
    , m_running(false)
{
    s_context.init(sf::Vector2f(960.f, 540.f), "Evily Evil Villains", sf::Style::Default);
    s_context.window.setKeyRepeatEnabled(false);
    s_context.window.setVerticalSyncEnabled(true);

    loadTextures();
    loadShaders();
    loadSounds();
    loadFonts();
    loadAnimations();

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

    if (s_context.window.isOpen())
        s_context.window.close();
}

void Application::processInput()
{
    sf::Event event;

    while (s_context.window.pollEvent(event)) {

        // Switch fullscreen mode
        if (event.type == sf::Event::KeyPressed
            && event.key.code == sf::Keyboard::F11) {
            switchFullscreenMode();
            return;
        }

        // Closing window
        if (event.type == sf::Event::Closed) {
            m_running = false;
            return;
        }

        // Resizing window
        if (event.type == sf::Event::Resized) {
            s_context.screenSize = sf::Vector2f(event.size.width, event.size.height);
            refresh();
            return;
        }

        m_stateStack.handleEvent(event);
    }
}

void Application::update(sf::Time dt)
{
    // Shaders can be animated
    m_gameTime += dt.asSeconds();

    // TODO Move to a shader code
    s_context.shaders.setParameter(Shaders::MENU_BG, "time", m_gameTime/5.f);
    s_context.shaders.setParameter(Shaders::MENU_NAME, "time", m_gameTime/5.f);
    s_context.animations.update(dt);
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
    // Splash-screen
    s_context.textures.load(Textures::JUMPINGTOASTS_BG, "res/tex/jumping-toasts/bg.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_JUMPING, "res/tex/jumping-toasts/jumping.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTS, "res/tex/jumping-toasts/toasts.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOAST, "res/tex/jumping-toasts/toast.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTCUT, "res/tex/jumping-toasts/toast-cut.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERBACKGROUND, "res/tex/jumping-toasts/toaster-background.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERBOTTOM, "res/tex/jumping-toasts/toaster-bottom.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERMIDDLE, "res/tex/jumping-toasts/toaster-middle.png");
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERTOP, "res/tex/jumping-toasts/toaster-top.png");

    s_context.textures.setSmooth(Textures::JUMPINGTOASTS_TOAST, false);
    s_context.textures.setSmooth(Textures::JUMPINGTOASTS_TOASTCUT, false);

    // NUI
    s_context.textures.load(Textures::NUI_FOCUS, "res/tex/nui/focus.png");

    // Menu
    s_context.textures.load(Textures::MENU_BG, "res/tex/menu/bg.png");
    s_context.textures.load(Textures::MENU_NAME, "res/tex/menu/name.png");
}

void Application::loadShaders()
{
    // NUI
    s_context.shaders.load(Shaders::NUI_HOVER, "res/shd/nui/hover.frag", sf::Shader::Fragment);
    s_context.shaders.load(Shaders::NUI_FOCUS, "res/shd/nui/focus.frag", sf::Shader::Fragment);

    // Menu
    s_context.shaders.load(Shaders::MENU_BG, "res/shd/menu/bg.vert", "res/shd/menu/bg.frag");
    s_context.shaders.load(Shaders::MENU_NAME, "res/shd/menu/name.vert", sf::Shader::Vertex);

    refreshShaders();
}

void Application::refreshShaders()
{
    const auto& screenSize = s_context.screenSize;
    const auto& resolution = s_context.resolution;
    const auto& effectiveDisplay = s_context.effectiveDisplay;

    // NUI
    s_context.shaders.setParameter(Shaders::NUI_HOVER, "texture", sf::Shader::CurrentTexture);
    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "screenSize", screenSize);
    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "resolution", resolution);
    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "effectiveDisplay", effectiveDisplay);

    // Menu
    s_context.shaders.setParameter(Shaders::MENU_BG, "screenSize", screenSize);
    s_context.shaders.setParameter(Shaders::MENU_BG, "resolution", resolution);
    s_context.shaders.setParameter(Shaders::MENU_BG, "effectiveDisplay", effectiveDisplay);
    s_context.shaders.setParameter(Shaders::MENU_BG, "texture", sf::Shader::CurrentTexture);
}

void Application::loadFonts()
{
    s_context.fonts.load(Fonts::NUI, "res/font/dream_orphans.ttf");
}

void Application::loadSounds()
{
    // Splash-screen
    s_context.sounds.load(Sounds::JUMPINGTOASTS, "res/snd/jumping-toasts.wav");

    // NUI
    s_context.sounds.load(Sounds::NUI_ACCEPT, "res/snd/accept.wav");
    s_context.sounds.load(Sounds::NUI_REFUSE, "res/snd/refuse.wav");
    s_context.sounds.load(Sounds::NUI_SELECT, "res/snd/select.wav");
}

void Application::loadAnimations()
{
    // Splash-screen
    s_context.animations.load(Animations::JUMPINGTOASTS, "res/scml/jumping-toasts.scml");
}

//------------------//
//----- States -----//

void Application::registerStates()
{
    m_stateStack.registerState<SplashScreenState>(States::SPLASHSCREEN);

    m_stateStack.registerState<MenuMainState>(States::MENU_MAIN);
    m_stateStack.registerState<MenuSelectWorldState>(States::MENU_SELECTWORLD);

    m_stateStack.registerState<GameDungeonDesignState>(States::GAME_DUNGEON_DESIGN);
    m_stateStack.registerState<GamePauseState>(States::GAME_PAUSE);
}

//-----------------------------//
//----- Window management -----//

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
    sf::Vector2f viewRatio = sf::vdiv(screenSize, resolution);

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

