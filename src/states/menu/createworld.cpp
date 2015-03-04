#include "states/menu/createworld.hpp"

#include "core/application.hpp"

MenuCreateWorldState::MenuCreateWorldState(StateStack& stack)
    : baseClass(stack)
{
    const auto& resolution = Application::context().resolution;

    // Background
    sceneLayer(LayerID::NUI).attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(resolution);
}

bool MenuCreateWorldState::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    return State::handleEvent(event);
}
