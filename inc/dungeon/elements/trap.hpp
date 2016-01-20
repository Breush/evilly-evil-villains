#pragma once

#include "dungeon/elements/element.hpp"
#include "dungeon/structs/room.hpp"

namespace dungeon
{
    //! A generic trap interface.

    class Trap final : public Element
    {
        using baseClass = Element;

    public:

        //! Constructor.
        Trap(const RoomCoords& coords, ElementData& edata, dungeon::Inter& inter);

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
        //! @name Lua API
        //! @{

        //! Indicate that harvestable dosh has changed.
        void lua_warnHarvestableDosh();

        //! @}

    protected:

        RoomCoords m_coords;    //!< The room in which the trap is set.
    };
}
