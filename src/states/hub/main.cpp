#include "states/hub/main.hpp"

#include "core/application.hpp"

#include <iostream>

using namespace states;

HubMain::HubMain(StateStack& stack, dungeon::Data& data)
    : baseClass(stack)
    , m_data(data)
    , m_selector(*this)
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

    // Selector
    nuiRoot.attachChild(m_selector);
    m_selector.setRelativePosition({0.5f, 0.5f});
    m_selector.setRelativeOrigin({0.5f, 0.5f});
    m_selector.setLocalScale(backgroundScale);
    m_selector.setSize(backgroundSize * backgroundScale);
    m_selector.init();
}

HubMain::~HubMain()
{
    // Freeing resources
    Application::freeAnimations({"hub"});
    Application::freeTextures({"hub"});
}

//-------------------//
//----- Routine -----//

bool HubMain::update(const sf::Time& dt)
{
    baseClass::update(dt);
    return false;
}

void HubMain::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}
