#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/sprite.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <list>

namespace states
{
    //! State shown at the start of the game.

    class SplashScreen final : public State
    {
    public:

        //! Constructor.
        SplashScreen(StateStack& stack);

        //! Destructor.
        ~SplashScreen();

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::SPLASHSCREEN; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;

        //! @}

    private:

        // Decorum
        sfe::Sprite m_background;   //!< Background.
        sfe::AnimatedSprite m_logo; //!< Company logo.
    };
}
