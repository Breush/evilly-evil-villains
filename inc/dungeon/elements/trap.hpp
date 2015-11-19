#pragma once

#include "dungeon/elements/element.hpp"

namespace dungeon
{
    //! A generic trap interface.

    class Trap final : public Element
    {
        using baseClass = Element;

    public:

        //! Constructor.
        Trap(const sf::Vector2u& coords, ElementData& edata, dungeon::Inter& inter);

        //! Default destructor.
        ~Trap() = default;

        std::string detectKey() const final { return "trap"; }
        std::string _name() const final { return "dungeon::Trap"; }

        //------------------//
        //! @name Resources
        //! @{

        //! Remove dosh from resources and return the value harvested.
        uint32 harvestDosh();

        //! How much dosh is harvestable from this trap.
        uint32 harvestableDosh();

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt) final;

        //! @}

        //----------------//
        //! @name Lua API
        //! @{

        //! Indicate that harvestable dosh has changed.
        void lua_warnHarvestableDosh();

        //! @}

    protected:

        sf::Vector2u m_coords;  //!< The room in which the trap is set.
    };
}
