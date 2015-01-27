#include "core/application.hpp"
#include "states/game/pause.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


GamePauseState::GamePauseState(StateStack& stack)
    : State(stack)
{
    const auto& font = Application::context().fonts.get(Fonts::NUI);
    const auto& resolution = Application::context().resolution;

    // Background
    m_bg.setFillColor(sf::Color(0, 0, 0, 150));
    m_bg.setSize(Application::context().resolution);

    // Pause text
    m_pausedText.setFont(font);
    m_pausedText.setString("Game Paused");
    m_pausedText.setCharacterSize(70);
    m_pausedText.setPosition(0.5f * resolution.x, 0.4f * resolution.y);
    auto bounds = m_pausedText.getLocalBounds();
    m_pausedText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);

    // Instruction
    m_instructionText.setFont(font);
    m_instructionText.setString("(Press Backspace to return to the main menu)");
    m_instructionText.setPosition(0.5f * resolution.x, 0.6f * resolution.y);
    bounds = m_instructionText.getLocalBounds();
    m_instructionText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);
}

void GamePauseState::draw()
{
    auto& window = Application::context().window;

    window.draw(m_bg);
    window.draw(m_pausedText);
    window.draw(m_instructionText);
}

bool GamePauseState::update(const sf::Time&)
{
    return false;
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

    return false;
}
