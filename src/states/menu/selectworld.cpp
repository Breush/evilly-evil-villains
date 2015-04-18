#include "states/menu/selectworld.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "tools/time.hpp"
#include "tools/tools.hpp"
#include "context/worlds.hpp"
#include "resources/identifiers.hpp"

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
    m_title.setCentered(true);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.setPrestyle(sfe::Label::Prestyle::MENU_TITLE);

    // List for existing worlds
    nuiRoot.attachChild(m_list);
    m_list.setCentered(true);
    m_list.setSize(0.75f * nuiSize);
    m_list.setRelativePosition({0.5f, 0.5f});
    m_list.setColumns({_("Villain"), _("World name"), _("Main dungeon"), _("Last played")});
    m_list.setColumnFillClip(3, false, false);

    // Load list of worlds
    // TODO Have time format within gettext so that each country can choose its own representation
    context::worlds.load();
    for (const auto& world : context::worlds.get())
        m_list.addLine({world.villain, world.name, world.mainDungeon, time2wstring("%Y-%m-%d", world.lastPlayed)});

    // Stacker for buttons
    nuiRoot.attachChild(m_stacker);
    m_stacker.setAlign(nui::Stacker::Align::CENTER);
    m_stacker.setSize({nuiSize.x, 0.95f * nuiSize.y});

    // Buttons
    for (auto& button : m_buttons)
        m_stacker.add(&button, nui::Stacker::Align::OPPOSITE);

    m_buttons[2].setAction(_("Play"), [this] { playOnSelectedWorld(); });
    m_buttons[1].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[0].setAction(_("Create new world"), [this] { stackPush(StateID::MENU_CREATEWORLD); });

    // Ambient feeling music
    Application::context().musics.setVolume(25);
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

    wdebug_application_1(L"Loading world " + worldInfo.name);

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}

