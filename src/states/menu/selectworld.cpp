#include "states/menu/selectworld.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "tools/time.hpp"
#include "tools/tools.hpp"
#include "context/worlds.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

using namespace states;

MenuSelectWorld::MenuSelectWorld(StateStack& stack)
    : baseClass(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Select a world"));
    m_title.setPrestyle(sfe::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // List for existing worlds
    nuiRoot.attachChild(m_list);
    m_list.centerOrigin();
    m_list.setSize({0.75f * nuiSize.x, 0.65f * nuiSize.y});
    m_list.setRelativePosition({0.5f, 0.5f});
    m_list.setColumnsTitles({_("Villain"), _("World name"), _("Main dungeon"), _("Last played")});
    m_list.setColumnAlign(3, nui::Align::OPPOSITE, nui::Align::CENTER);
    m_list.setColumnAdapt(3, nui::Adapt::FIT);

    // Load list of worlds
    context::worlds.load();
    for (const auto& world : context::worlds.get())
        m_list.addLine({world.villain, world.name, world.mainDungeon, time2wstring(toString(_("%Y-%m-%d")), world.lastPlayed)});
    m_list.setActionCallback([this] { playOnSelectedWorld(); });

    // Stacker for buttons
    nuiRoot.attachChild(m_stacker);
    m_stacker.setRelativeOrigin({0.5f, 1.f});
    m_stacker.setRelativePosition({0.5f, 0.95f});

    // Buttons
    for (auto& button : m_buttons)
        m_stacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[2].setAction(_("Play"), [this] { playOnSelectedWorld(); });
    m_buttons[1].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[0].setAction(_("Create new world"), [this] { stackPush(StateID::MENU_CREATEWORLD); });
}

bool MenuSelectWorld::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    return State::handleEvent(event);
}

void MenuSelectWorld::playOnSelectedWorld()
{
    auto selectedWorld = m_list.selectedLine();
    auto& worldInfo = context::worlds.select(selectedWorld);
    stackClear(StateID::GAME_DUNGEON_DESIGN);
}

