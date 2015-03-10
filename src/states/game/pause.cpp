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

    // Background
    sceneLayer(LayerID::NUI).attachChild(m_background);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(Application::context().resolution);

    // Pause text
    sceneLayer(LayerID::NUI).attachChild(m_pauseText);
    m_pauseText.setFont(FontID::NUI);
    m_pauseText.setText(_("Game paused"));
    m_pauseText.setCharacterSize(50);
    m_pauseText.setPosition(0.5f * resolution.x, 0.4f * resolution.y);
    m_pauseText.setCentered(true);

    // Buttons
    sceneLayer(LayerID::NUI).attachChild(m_continueButton);
    m_continueButton.setAction(_("Continue, continue, continue"), [this]() { stackPop(); });
    m_continueButton.setLocalPosition(0.5f * resolution);
    m_continueButton.setCentered(true);

    sceneLayer(LayerID::NUI).attachChild(m_mainMenuButton);
    m_mainMenuButton.setAction(_("Save and return to main menu"), [this]() { stackClear(StateID::MENU_MAIN); });
    m_mainMenuButton.setLocalPosition({0.5f * resolution.x, 0.6f * resolution.y});
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
