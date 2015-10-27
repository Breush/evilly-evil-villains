#include "states/game/over.hpp"

#include "core/gettext.hpp"
#include "tools/tools.hpp"

using namespace states;

GameOver::GameOver(StateStack& stack)
    : State(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Title
    nuiRoot.attachChild(m_gameOver);
    m_gameOver.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_gameOver.setRelativePosition({0.5f, 0.5f});
    m_gameOver.centerOrigin();
}

GameOver::~GameOver()
{
}

//-------------------//
//----- Routine -----//

bool GameOver::update(const sf::Time& dt)
{
    return baseClass::update(dt);
}

void GameOver::handleEvent(const sf::Event& event)
{
    // Back to main menu on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackClear(StateID::MENU_MAIN);
        return;
    }

    State::handleEvent(event);
}

void GameOver::refreshWindow(const config::WindowInfo& cWindow)
{
    baseClass::refreshWindow(cWindow);

    // Translated strings
    m_gameOver.setText(_("Game Over"));
}
