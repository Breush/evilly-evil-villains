#include "states/splashscreen.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

using namespace states;

SplashScreen::SplashScreen(StateStack& stack)
    : State(stack)
{
    // Loading resources
    Application::loadTextures({"jumping-toasts"});
    Application::loadAnimations({"jumping-toasts"});
    Application::context().textures.get("jumping-toasts/background").setRepeated(true);
    Application::context().textures.get("jumping-toasts/toast-cut").setSmooth(false);

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    float maxSide = std::max(nuiSize.x, nuiSize.y);

    // Background
    const auto& textureSize = Application::context().textures.get("jumping-toasts/background").getSize();
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture("jumping-toasts/background");
    m_background.setShader("menu/splashscreen");
    m_background.setLocalScale(maxSide / sf::v2f(textureSize));
    m_background.setLocalPosition((nuiSize - maxSide) / 2.f);

    // Animation
    nuiRoot.attachChild(m_logo);
    m_logo.load("jumping-toasts/logo");
    m_logo.setRelativePosition({0.5f, 0.5f});
    m_logo.setLooping(false);
    m_logo.restart();
}

SplashScreen::~SplashScreen()
{
    // Freeing resources
    Application::freeTextures({"jumping-toasts"});
    Application::freeAnimations({"jumping-toasts"});
}

//-------------------//
//----- Routine -----//

bool SplashScreen::update(const sf::Time& dt)
{
    // Check on animated entities
    if (!m_logo.started())
        stackReplace(StateID::MENU_MAIN);

    return State::update(dt);
}

void SplashScreen::handleEvent(const sf::Event& event)
{
    // Skip splashscreen on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        Application::context().sounds.stopAll();
        stackReplace(StateID::MENU_MAIN);
        return;
    }

    State::handleEvent(event);
}
