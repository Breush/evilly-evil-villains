#include "states/hub/hub.hpp"

#include "core/application.hpp"

using namespace states;

Hub::Hub(StateStack& stack)
    : baseClass(stack)
{
    // Loading resources
    Application::loadTextures({"hub"});
    Application::loadAnimations({"hub"});

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    const auto nuiRatio = nuiSize.x / nuiSize.y;

    // Background
    const auto backgroundSize = sf::v2f(Application::context().textures.get("hub/day/back").getSize());
    const auto backgroundRatio = backgroundSize.x / backgroundSize.y;
    float backgroundScaleValue = 1.f;
    if (nuiRatio > backgroundRatio) backgroundScaleValue *= nuiSize.x / backgroundSize.x;
    else                            backgroundScaleValue *= nuiSize.y / backgroundSize.y;
    const sf::Vector2f backgroundScale(backgroundScaleValue, backgroundScaleValue);

    nuiRoot.attachChild(m_scene);
    m_scene.load("hub/hub");
    m_scene.setRelativePosition({0.5f, 0.5f});
    m_scene.setLocalScale(backgroundScale);
}

Hub::~Hub()
{
    // Freeing resources
    Application::freeAnimations({"hub"});
    Application::freeTextures({"hub"});
}

//-------------------//
//----- Routine -----//

bool Hub::update(const sf::Time& dt)
{
    baseClass::update(dt);
    return false;
}

void Hub::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}
