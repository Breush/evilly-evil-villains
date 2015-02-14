#include "states/game/pause.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

// TODO What to do with this state? Open config menu?
// At least, make it use the nui tools (buttons)
// And make strings translatable

GamePauseState::GamePauseState(StateStack& stack)
    : State(stack)
{
    const auto& resolution = Application::context().resolution;

    // Background
    sceneLayer(Layers::NUI).attachChild(m_background);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(Application::context().resolution);

    // Pause text
    sceneLayer(Layers::NUI).attachChild(m_pauseText);
    m_pauseText.setFont(Fonts::NUI);
    m_pauseText.setText(L"Game Paused");
    m_pauseText.setCharacterSize(70);
    m_pauseText.setPosition(0.5f * resolution.x, 0.4f * resolution.y);
    m_pauseText.setCentered(true);

    // Instruction TODO Use UI to access options etc.
    sceneLayer(Layers::NUI).attachChild(m_instructionText);
    m_instructionText.setFont(Fonts::NUI);
    m_instructionText.setText(L"(Press Backspace to return to the main menu)");
    m_instructionText.setPosition(0.5f * resolution.x, 0.6f * resolution.y);
    m_instructionText.setCentered(true);
}

bool GamePauseState::handleEvent(const sf::Event& event)
{
    returnif (event.type != sf::Event::KeyPressed) false;

    // Back to previous state
    if (event.key.code == sf::Keyboard::Escape)
        stackPop();

    // Return to main menu
    else if (event.key.code == sf::Keyboard::BackSpace)
        stackClear(States::MENU_MAIN);

    return State::handleEvent(event);
}
