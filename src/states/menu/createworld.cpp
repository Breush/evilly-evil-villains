#include "states/menu/createworld.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"

MenuCreateWorldState::MenuCreateWorldState(StateStack& stack)
    : baseClass(stack)
{
    const auto& resolution = Application::context().resolution;

    // Background
    sceneLayer(LayerID::NUI).attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor({0, 0, 0, 230});
    m_background.setSize(resolution);

    // NUI
    sceneLayer(LayerID::NUI).attachChild(m_stacker);
    m_stacker.setAlign(nui::Stacker::Align::CENTER);
    m_stacker.setSize({resolution.x, 100.f});

    // Stacking
    m_stacker.add(&m_worldNameLabel, nui::Stacker::Align::CENTER);
    m_worldNameLabel.setText(_("World name"), FontID::NUI, 16);

    m_stacker.add(&m_worldNameEntry, nui::Stacker::Align::CENTER);
    m_worldNameEntry.setSize({resolution.x * 0.4f, 25.f});
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
