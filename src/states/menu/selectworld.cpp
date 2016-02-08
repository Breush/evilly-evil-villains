#include "states/menu/selectworld.hpp"

#include "context/context.hpp"
#include "context/logger.hpp"
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
    m_background.setPointsColors({0u, 0u, 0u, 210u}, {0u, 0u, 0u, 255u});
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Select a world"));
    m_title.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // List for existing worlds
    nuiRoot.attachChild(m_list);
    m_list.centerOrigin();
    m_list.setSize(nuiSize * sf::Vector2f{0.75f, 0.65f});
    m_list.setRelativePosition({0.5f, 0.5f});
    m_list.setColumnsTitles({_("Villain"), _("Dungeon name"), _("Last played")});
    m_list.setColumnAlign(2u, nui::Align::OPPOSITE, nui::Align::CENTER);
    m_list.setColumnAdapt(2u, nui::Adapt::FIT);

    // Load list of worlds
    context::worlds.load();
    for (const auto& world : context::worlds.get())
        m_list.addLine({world.villain, world.name, time2wstring(toString(_("%Y-%m-%d")), world.lastPlayed)});
    m_list.setActionCallback([this] { playOnSelectedWorld(); });

    // Stacker for buttons
    nuiRoot.attachChild(m_stacker);
    m_stacker.setRelativeOrigin({0.5f, 1.f});
    m_stacker.setRelativePosition({0.5f, 0.95f});

    // Buttons
    for (auto& button : m_buttons)
        m_stacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[2].set(_("Play"), [this] { playOnSelectedWorld(); });
    m_buttons[1].set(_("Back"), [this] { stackPop(); });
    m_buttons[0].set(_("New game"), [this] { stackPush(StateID::MENU_NEWGAME); });
}

//-------------------//
//----- Routine -----//

void MenuSelectWorld::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}

//-----------------------//
//----- Interpreter -----//

context::CommandPtr MenuSelectWorld::interpret(std::vector<std::wstring>& tokens)
{
    std::wstring logMessage;
    auto nTokens = tokens.size();

    if (nTokens == 2u) {
        if (tokens[0u] == L"start") {
            logMessage = L"> [menuSelectWorld] Playing on world " + tokens[1u];
            auto line = to<uint>(tokens[1u]);
            m_list.selectLine(line);
            playOnSelectedWorld();
        }
    }

    if (logMessage.empty()) return nullptr;

    auto pCommand = std::make_unique<context::Command>();
    context::setCommandLog(*pCommand, logMessage);
    return std::move(pCommand);
}

void MenuSelectWorld::autoComplete(std::vector<std::wstring>& possibilities, const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        if (std::wstring(L"start").find(lastToken) == 0u)    possibilities.emplace_back(L"start");
    }
}

//-----------------------------//
//----- Worlds management -----//

void MenuSelectWorld::playOnSelectedWorld()
{
    if (m_list.empty()) return;

    auto selectedWorld = m_list.selectedLine();
    context::worlds.select(selectedWorld);

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}

