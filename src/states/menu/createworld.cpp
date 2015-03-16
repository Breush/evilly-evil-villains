#include "states/menu/createworld.hpp"

#include "core/gettext.hpp"
#include "world/context.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/filesystem.hpp"
#include "tools/string.hpp"
#include "resources/identifiers.hpp"

using namespace states;

MenuCreateWorld::MenuCreateWorld(StateStack& stack)
    : baseClass(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor({0, 0, 0, 230});
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Create a new world"));
    m_title.setCentered(true);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.setPrestyle(sfe::Label::PrestyleID::MENU_TITLE);

    // NUI
    nuiRoot.attachChild(m_stacker);
    m_stacker.setAlign(nui::Stacker::Align::CENTER);
    m_stacker.setRelativePosition({0.f, 0.125f});
    m_stacker.setSize({nuiSize.x, 50.f});

    // Stacking
    m_stacker.add(&m_worldNameLabel, nui::Stacker::Align::CENTER);
    m_worldNameLabel.setText(_("World name"), FontID::NUI, 16);

    m_stacker.add(&m_worldNameEntry, nui::Stacker::Align::CENTER);
    m_worldNameEntry.setLength(nuiSize.x * 0.4f);

    // Buttons
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setAlign(nui::Stacker::Align::CENTER);
    m_buttonsStacker.setSize({nuiSize.x, 0.95f * nuiSize.y});

    for (auto& button : m_buttons)
        m_buttonsStacker.add(&button, nui::Stacker::Align::OPPOSITE);

    m_buttons[0].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[1].setAction(_("Create and start playing"), [this] { createAndPlayWorld(); });
}

bool MenuCreateWorld::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    return State::handleEvent(event);
}

void MenuCreateWorld::createAndPlayWorld()
{
    auto worldName = m_worldNameEntry.text();
    returnif (worldName.empty());

    wdebug_application_1(L"Creating world " + worldName);

    // auto& worldData = world::context.worldsData;

    // Creating folder
    auto folder(worldName);
    filterSpecial(folder);

    if (!createDirectory(L"worlds/" + folder)) {
        // TODO Manage user feedback (or automatically rename folder)
        std::wcout << L"Cannot create directory worlds/" << worldName << std::endl;
        return;
    }

    // TODO Do something clever
    // stackClear(StateID::GAME_DUNGEON_DESIGN)
    stackPop();
}