#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"
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

        // Fading
        scene::RectangleShape m_fadingRectangle;            //!< Smooth transition to the next state.
        sf::Time m_fadingTime = sf::Time::Zero;             //!< Current time for fading effect.
        const sf::Time m_fadingDelay = sf::seconds(0.75f);  //!< How much time to wait for fading.

        // Decorum
        scene::Sprite m_background;     //!< Background.
        scene::AnimatedSprite m_logo;   //!< Company logo.
    };
}
