#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "sfe/animatedsprite.hpp"
#include "sfe/sprite.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <list>

namespace states
{
    class SplashScreen : public State
    {
    public:
        SplashScreen(StateStack& stack);
        virtual ~SplashScreen() = default;
        StateID id() const noexcept override { return StateID::SPLASHSCREEN; }

        // Routines
        bool update(const sf::Time& dt) override;
        bool handleEvent(const sf::Event& event) override;

    private:
        sfe::Sprite m_background;
        sfe::AnimatedSprite m_logo;
    };
}
