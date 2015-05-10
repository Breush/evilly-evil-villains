#pragma once

#include "dungeon/traps.hpp"
#include "dungeon/event.hpp"

namespace dungeon
{
namespace traps
{
    //!< A pick-pock steals money from passing by heroes.

    class PickPock final : public Trap, public EventReceiver
    {
        using baseClass = Trap;

    public:

        //! Constructor.
        PickPock(const Data::Room& room);

        //! Default destructor.
        ~PickPock() = default;

        //------------------//
        //! @name Resources
        //! @{

        inline uint harvestableDosh() const final { return dosh(); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

        //-----------------//
        //! @name Stealing
        //! @{

        void stealFromHero(Hero* hero);

        //! @}

    private:

        uint m_maxDosh = 20u;           //!< The max amount of dosh this trap can hold.

        // Decorum
        sf::RectangleShape m_sprite;    //!< The sprite.
    };
}
}
