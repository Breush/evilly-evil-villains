#include "states/menu/createworld.hpp"

#include "core/gettext.hpp"
#include "world/context.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
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

    // World name
    nuiRoot.attachChild(m_worldNameStacker);
    m_worldNameStacker.setAlign(nui::Stacker::Align::CENTER);
    m_worldNameStacker.setLocalPosition({0.f, 135.f});
    m_worldNameStacker.setSize({nuiSize.x, 50.f});

    m_worldNameStacker.add(&m_worldNameLabel, nui::Stacker::Align::CENTER);
    m_worldNameStacker.add(&m_worldNameEntry, nui::Stacker::Align::CENTER);
    m_worldNameLabel.setText(_("World name"), FontID::NUI);
    m_worldNameEntry.setLength(20u);

    // Villain
    nuiRoot.attachChild(m_villainStacker);
    m_villainStacker.setAlign(nui::Stacker::Align::CENTER);
    m_villainStacker.setLocalPosition({0.f, 185.f});
    m_villainStacker.setSize({nuiSize.x, 50.f});

    m_villainStacker.add(&m_villainLabel, nui::Stacker::Align::CENTER);
    m_villainStacker.add(&m_villainBox, nui::Stacker::Align::CENTER);
    m_villainLabel.setText(_("Villain"), FontID::NUI);

    // FIXME List all villains from somewhere
    m_villainBox.add(L"Breush", nullptr);
    m_villainBox.add(L"The Revolted Waffle", nullptr);
    m_villainBox.add(L"Le Précurseur", nullptr);

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
    auto villain = m_villainBox.selectedChoiceText();
    returnif (worldName.empty());

    wdebug_application_1(L"Creating and playing on world " + worldName + L" with villain " + villain);

    // World data
    auto newWorldID = world::context.createWorld(worldName, villain);
    world::context.selectWorld(newWorldID);

    stackClear(StateID::GAME_DCB);
}
