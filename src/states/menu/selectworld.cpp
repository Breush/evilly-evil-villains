#include "states/menu/selectworld.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "world/context.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


MenuSelectWorldState::MenuSelectWorldState(StateStack& stack)
    : baseClass(stack)
    , m_uiCore(&m_mouseDetector)
{
    auto& resolution = Application::context().resolution;

    // Hovering system
    m_mouseDetector.init();

    // Background
    m_bgRect.setFillColor(sf::Color(0, 0, 0, 230));
    m_bgRect.setSize(resolution);

    // List for existing worlds
    m_uiCore.add(&m_list);
    m_list.setLocalPosition(0.1f * resolution);
    m_list.setSize({0.8f * resolution.x, 0.7f * resolution.y});
    m_list.setColumns({_("Villain"), _("World name"), _("Main dungeon"), _("Last played")});
    m_list.setColumnFillClip(2, false, false);

    // Load list of worlds
    // TODO Choose which columns to show (more?)
    // TODO Have time format within gettext so that each country can choose its own representation
    world::context.worldsData.load("worlds/worlds.xml");
    for (const auto& world : world::context.worldsData.worlds())
        m_list.addLine({world.villain, world.name, world.mainDungeon, time2wstring("%F", world.lastPlayed)});

    // Stacker for buttons
    m_uiCore.add(&m_stacker);
    m_stacker.setAlign(nui::Stacker::CENTER);
    m_stacker.setSize({resolution.x, 0.95f * resolution.y});

    // Buttons
    for (auto& button : m_buttons)
        m_stacker.add(&button, nui::Stacker::OPPOSITE);

    m_buttons[2].setAction(_("Play"), [this]() {
        auto selectedWorld = m_list.selectedLine();
        auto& worldInfo = world::context.worldsData.worlds()[selectedWorld];
        wdebug_application_1(L"Loading world " + worldInfo.name);
        world::context.info = &worldInfo;
        stackClear(States::GAME_DUNGEON_DESIGN);
    });
    m_buttons[1].setAction(_("Back"), [this]() { stackPop(); });
    m_buttons[0].setAction(_("Create new world"), nullptr);

    m_stacker.update();

    // Ambient feeling music
    Application::context().music.setVolume(25);
}

void MenuSelectWorldState::draw()
{
    auto& window = Application::context().window;

    // Background
    window.draw(m_bgRect);

    // Menu ui
    window.draw(m_uiCore);

    // Hovering system
    m_mouseDetector.draw();
}

bool MenuSelectWorldState::update(const sf::Time& dt)
{
    m_uiCore.update(dt);
    m_mouseDetector.update(dt);

    return true;
}

bool MenuSelectWorldState::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    // Let ui core handle events
    m_uiCore.handleEvent(event);

    return false;
}

void MenuSelectWorldState::refresh()
{
    m_uiCore.refresh();
}

