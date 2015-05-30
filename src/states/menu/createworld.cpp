#include "states/menu/createworld.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "context/worlds.hpp"
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
    m_title.setPrestyle(sfe::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // Form
    nuiRoot.attachChild(m_form);
    m_form.centerOrigin();
    m_form.setSize({500.f, 500.f});
    m_form.setRelativePosition({0.5f, 0.5f});

    // World name
    m_form.add(_("World name"), m_worldNameEntry);
    m_worldNameEntry.setLength(20u);

    // Villain
    m_form.add(_("Villain"), m_villainBox);
    context::villains.load();
    for (const auto& villain : context::villains.get())
        m_villainBox.add(villain.name, nullptr);

    // Buttons
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setRelativeOrigin({0.5f, 1.f});
    m_buttonsStacker.setRelativePosition({0.5f, 0.95f});

    for (auto& button : m_buttons)
        m_buttonsStacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[1].setAction(_("Create and start playing"), [this] { createAndPlayWorld(); });
}

//------------------//
//----- Events -----//

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

//-------------------------------//
//----- Villains management -----//

void MenuCreateWorld::createAndPlayWorld()
{
    auto worldName = m_worldNameEntry.text();
    auto villain = m_villainBox.selectedChoiceText();
    returnif (worldName.empty());

    wdebug_application_1(L"Creating and playing on world " + worldName + L" with villain " + villain);

    // World data
    auto newWorldID = context::worlds.add(worldName, villain);
    context::worlds.select(newWorldID);

    stackClear(StateID::GAME_DCB);
}
