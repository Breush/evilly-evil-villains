#include "states/game/donjondesign.hpp"

#include "resources/musicplayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameDonjonDesignState::GameDonjonDesignState(StateStack& stack, Context context)
    : State(stack, context)
    , m_uiCore(context)
{
    auto& viewSize = context.window->getView().getSize();

    // Stop music if any
    context.music->stop();

    // Donjon Inter
    m_uiCore.add(&m_donjonInter);
    m_donjonInter.setSize({350, 450});
    m_donjonInter.setPosition((viewSize - m_donjonInter.size()) / 2.f);
}

void GameDonjonDesignState::draw()
{
    sf::RenderWindow& window = *getContext().window;

    window.draw(m_donjonInter);
}

bool GameDonjonDesignState::update(sf::Time dt)
{
    m_uiCore.update(dt);
    return true;
}

bool GameDonjonDesignState::handleEvent(const sf::Event& event)
{
    // Escape pressed, trigger the pause screen
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            stackPush(States::GAME_PAUSE);
        }
    }

    // Let ui core handle events
    m_uiCore.handleEvent(event);
    return false;
}

