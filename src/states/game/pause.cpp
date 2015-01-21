#include "core/application.hpp"
#include "states/game/pause.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


GamePauseState::GamePauseState(StateStack& stack)
    : State(stack)
    , mBackgroundSprite()
    , mPausedText()
    , mInstructionText()
{
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    sf::Vector2f viewSize = Application::context().resolution;

    mPausedText.setFont(font);
    mPausedText.setString("Game Paused");
    mPausedText.setCharacterSize(70);
    mPausedText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);
    auto bounds = mPausedText.getLocalBounds();
    mPausedText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);

    mInstructionText.setFont(font);
    mInstructionText.setString("(Press Backspace to return to the main menu)");
    mInstructionText.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
    bounds = mInstructionText.getLocalBounds();
    mInstructionText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);
}

void GamePauseState::draw()
{
    sf::RenderWindow& window = Application::context().window;

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(Application::context().resolution);

    window.draw(backgroundShape);
    window.draw(mPausedText);
    window.draw(mInstructionText);
}

bool GamePauseState::update(sf::Time)
{
    return false;
}

bool GamePauseState::handleEvent(const sf::Event& event)
{
    if(event.type != sf::Event::KeyPressed) {
        return false;
    }

    if(event.key.code == sf::Keyboard::Escape) {
        // Escape pressed, remove itself to return to the game
        stackPop();
    }

    if(event.key.code == sf::Keyboard::BackSpace) {
        // Escape pressed, remove itself to return to the game
        stackClear();
        stackPush(States::MENU_MAIN);
    }

    return false;
}
