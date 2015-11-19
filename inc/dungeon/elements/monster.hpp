#pragma once

#include "dungeon/elements/movingelement.hpp"

namespace dungeon
{
    //! A generic monster interface.

    class Monster final : public MovingElement
    {
        using baseClass = MovingElement;

    public:

        //! Constructor.
        Monster(Inter& inter, Graph& graph);

        //! Default destructor.
        ~Monster() = default;

        std::string detectKey() const final { return "monster"; }
        std::string _name() const final { return "dungeon::Monster"; }

    protected:

        //---------------------//
        //! @name Element data
        //! @{

        void rebindElementData() final;

        //! @}
    };
}
