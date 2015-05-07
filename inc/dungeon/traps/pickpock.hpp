#pragma once

#include "dungeon/traps.hpp"

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

    private:

        // Decorum
        sf::RectangleShape m_sprite;    //!< The sprite.
    };
}
}
