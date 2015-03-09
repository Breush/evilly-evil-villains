#include "states/menu/createworld.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "world/context.hpp"
#include "tools/debug.hpp"
#include "tools/filesystem.hpp"
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

    // Title
    sceneLayer(LayerID::NUI).attachChild(m_title);
    m_title.setText(_("Create a new world"));
    m_title.setCentered(true);
    m_title.setLocalPosition({resolution.x / 2.f, 40.f});
    m_title.setPrestyle(sfe::Label::PrestyleID::MENU_TITLE);

    // NUI
    sceneLayer(LayerID::NUI).attachChild(m_stacker);
    m_stacker.setAlign(nui::Stacker::Align::CENTER);
    m_stacker.setLocalPosition({0.f, 0.125f * resolution.y});
    m_stacker.setSize({resolution.x, 50.f});

    // Stacking
    m_stacker.add(&m_worldNameLabel, nui::Stacker::Align::CENTER);
    m_worldNameLabel.setText(_("World name"), FontID::NUI, 16);

    m_stacker.add(&m_worldNameEntry, nui::Stacker::Align::CENTER);
    m_worldNameEntry.setLength(resolution.x * 0.4f);

    // Buttons
    sceneLayer(LayerID::NUI).attachChild(m_buttonsStacker);
    m_buttonsStacker.setAlign(nui::Stacker::Align::CENTER);
    m_buttonsStacker.setSize({resolution.x, 0.95f * resolution.y});

    for (auto& button : m_buttons)
        m_buttonsStacker.add(&button, nui::Stacker::Align::OPPOSITE);

    m_buttons[0].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[1].setAction(_("Create and start playing"), [this] { createAndPlayWorld(); });
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

void MenuCreateWorldState::createAndPlayWorld()
{
    auto worldName = m_worldNameEntry.text();
    returnif (worldName.empty());

    wdebug_application_1(L"Creating world " + worldName);

    // auto& worldData = world::context.worldsData;

    if (!createDirectory(L"worlds/" + worldName)) {
        // TODO Manage user feedback (or automatically rename folder)
        std::wcout << L"Cannot create directory worlds/" << worldName << std::endl;
        return;
    }

    // TODO Do something clever
    // stackClear(StateID::GAME_DUNGEON_DESIGN)
    stackPop();
}
