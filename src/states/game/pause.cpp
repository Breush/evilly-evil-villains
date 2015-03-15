#include "states/game/pause.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

// TODO What to do with this state? Open config menu?
// At least, make it use the nui tools (buttons)
// And make strings translatable

using namespace states;

GamePause::GamePause(StateStack& stack)
    : State(stack)
{
    const auto& resolution = Application::context().resolution;

    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(resolution);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Game paused"));
    m_title.setCentered(true);
    m_title.setRelativePosition({0.5f, 0.35f});
    m_title.setPrestyle(sfe::Label::PrestyleID::MENU_POPUP_TITLE);

    // Buttons
    nuiRoot.attachChild(m_continueButton);
    m_continueButton.setAction(_("Continue, continue, continue"), [this]() { stackPop(); });
    m_continueButton.setRelativePosition({0.5f, 0.5f});
    m_continueButton.setCentered(true);

    nuiRoot.attachChild(m_mainMenuButton);
    m_mainMenuButton.setAction(_("Save and return to main menu"), [this]() { stackClear(StateID::MENU_MAIN); });
    m_mainMenuButton.setRelativePosition({0.5f, 0.6f});
    m_mainMenuButton.setCentered(true);
}

bool GamePause::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    return State::handleEvent(event);
}
