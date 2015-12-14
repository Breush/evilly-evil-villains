#pragma once

#include "dungeon/elements/element.hpp"

namespace dungeon
{
    // Forward declarations

    class FacilityInfo;
    class MovingElement;

    //! A generic facility interface.

    class Facility final : public Element
    {
        using baseClass = Element;

    public:

        //! Constructor.
        Facility(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter);

        //! Default destructor.
        ~Facility() = default;

        std::string detectKey() const final { return "facility"; }
        std::string _name() const final { return "dungeon::Facility"; }

        //---------------------------//
        //! @name Elements callbacks
        //! @{

        //! Warning about a element entering this facility tunnel.
        void movingElementEnterTunnel(MovingElement& movingElement);

        //! Warning about a element leaving this facility tunnel.
        void movingElementLeaveTunnel(MovingElement& movingElement);

        //! @}

        //----------------------------//
        //! @name Getters and setters
        //! @{

        //! Access the info used.
        inline const FacilityInfo& info() const { return m_facilityInfo; }

        //! @}

    protected:

        //----------------//
        //! @name Lua API
        //! @{

        //! Does a facility with the specified ID exists in the same room?
        bool lua_hasSiblingFacility(const std::string& facilityID) const;

        //! Get the current room x coordinate.
        uint32 lua_getCurrentRoomX() const;

        //! Get the current room y coordinate.
        uint32 lua_getCurrentRoomY() const;

        //----- Tresure

        //! Whether the facility currently hold treasure.
        bool lua_hasTreasure() const;

        //! Set the treasure held by the facility.
        void lua_setTreasure(const uint32 value);

        //----- Links

        //! Get the state of this facility: are we created as a link?
        bool lua_isLink();

        //! Has the facility an absolute link to an other room?
        bool lua_hasLink() const;

        //! The x coordinate of the absolute link. (Return -1u if none.)
        uint32 lua_getLinkRoomX() const;

        //! The x coordinate of the absolute link. (Return -1u if none.)
        uint32 lua_getLinkRoomY() const;

        //----- Tunnels

        //! Checks if a tunnel exists.
        bool lua_hasTunnel() const;

        //! Add a tunnel.
        void lua_addTunnel(const int32 x, const int32 y, bool relative);

        //! Remove all tunnels.
        void lua_removeTunnels();

        //----- Barrieres

        //! Checks if a barrier exists.
        bool lua_hasBarrier() const;

        //! Set the barrier state.
        void lua_setBarrier(bool activated);

        //! @}

    private:

        sf::Vector2u m_coords;          //!< The room in which the facility is set.
        FacilityInfo& m_facilityInfo;   //!< The facility data reference.
    };
}
