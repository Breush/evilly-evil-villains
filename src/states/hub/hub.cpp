#include "states/hub/hub.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

#include <iostream>

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

    // Did we moved over an hitbox?
    if (event.type == sf::Event::MouseMoved) {
        // Where's the mouse
        const auto& window = Application::context().window;
        auto mousePos = mousePosition(event);
        auto nuiPos = window.mapPixelToCoords(mousePos, nuiLayer().view());

        // Find if over any hitbox
        refreshHitboxSelected(nuiPos);
        return;
    }

    State::handleEvent(event);
}

//---------------//
//----- ICU -----//

void Hub::refreshHitboxSelected(const sf::Vector2f& nuiPos)
{
    auto findAndSelect = [this, &nuiPos] (const std::string& boxName)
    {
        auto boxBounds = m_scene.findBox(boxName + "_hitbox") + m_scene.getPosition();
        returnif (!boxBounds.contains(nuiPos)) false;
        m_scene.select(toWString(boxName) + L"_selected");
        return true;
    };

    returnif (findAndSelect("inn"));
    returnif (findAndSelect("bank"));
    returnif (findAndSelect("market"));
    returnif (findAndSelect("marketing_office"));

    // If none found, select default animation
    m_scene.select(L"none");
}
