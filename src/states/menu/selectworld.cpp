#include "states/menu/selectworld.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


MenuSelectWorldState::MenuSelectWorldState(StateStack& stack)
    : baseClass(stack)
    , m_uiCore()
{
    auto& viewSize = Application::context().resolution;

    // Background
    m_bgRect.setFillColor(sf::Color(0, 0, 0, 230));
    m_bgRect.setSize(viewSize);

    // List for existing worlds
    m_uiCore.add(&m_list);
    m_list.setPosition(0.1f * viewSize.x, 0.1f * viewSize.y);
    m_list.setSize({0.8f * viewSize.x, 0.7f * viewSize.y});
    m_list.setColumns({_("World name"), _("Villain"), _("Last played")});
    m_list.setColumnFillClip(2, false, false);

    // TODO Load list of worlds
    m_list.addLine({L"Petit", L"Homme", L"1er mai 1991 - 10:10"});
    m_list.addLine({L"Black chocolate", L"Breush", L"23 mai 2014 - 10:40"});
    m_list.addLine({L"Ceci est peut-être un peu long", L"Breush", L"23 mai 2014 - 10:40"});
    m_list.addLine({L"Miami", L"Breush qui dépasse un chouillas mais de rien", L"23 mai 2014 - 10:40"});
    m_list.addLine({L"Super long nom de monde tellement abusé que j'en ai les yeux qui me chatouillent la gorge",
                    L"Super Breush de la mort qui tue", L"31 avril 2013 - 01:23"});

    // Stacker for buttons
    m_uiCore.add(&m_stacker);
    m_stacker.setAlign(nui::Stacker::CENTER);
    m_stacker.setSize({viewSize.x, 0.95f * viewSize.y});

    // Buttons
    for (auto& button : m_buttons) {
        button = new nui::Button();
        m_stacker.add(button, nui::Stacker::OPPOSITE);
    }

    m_buttons[2]->setAction(_("Play"), [this]() {
        stackClear();
        stackPush(States::GAME_DONJON_DESIGN);
    });
    m_buttons[1]->setAction(_("Back"), [this]() {
        stackPop();
    });
    m_buttons[0]->setAction(_("Create new world"), [this]() {
        stackPop();
    });

    // Ambient feeling music
    Application::context().music.setVolume(25);
}

MenuSelectWorldState::~MenuSelectWorldState()
{
    for (auto& button : m_buttons)
        if (button != nullptr)
            delete button;

    // Ambient feeling music
    Application::context().music.setVolume(75);
}

void MenuSelectWorldState::draw()
{
    // Background
    Application::context().window.draw(m_bgRect);

    // Menu ui
    Application::context().window.draw(m_uiCore);
}

bool MenuSelectWorldState::update(sf::Time dt)
{
    m_uiCore.update(dt);
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

