#pragma once

#include "dungeon/elements/element.hpp"
#include "dungeon/structs/room.hpp"

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
        Facility(const RoomCoords& coords, ElementData& edata, Inter& inter);

        //! Destructor.
        ~Facility();

        std::string detectKey() const final { return "facility"; }
        std::string _name() const final { return "dungeon::Facility"; }

        //---------------------//
        //! @name Element data
        //! @{

        //! Access the coords the facility is in.
        inline const RoomCoords& coords() const { return m_coords; }

        //! Binds the element data.
        void bindFacilityInfo(FacilityInfo& facilityInfo);

        //! Access the element data.
        inline FacilityInfo& facilityInfo() { return *m_facilityInfo; }

        //! @}

        //---------------------------//
        //! @name Elements callbacks
        //! @{

        //! Warning about a element entering this facility tunnel.
        void movingElementEnterTunnel(MovingElement& movingElement);

        //! Warning about a element leaving this facility tunnel.
        void movingElementLeaveTunnel(MovingElement& movingElement);

        //! @}

    protected:

        //----------------//
        //! @name Lua API
        //! @{

        //! Does a facility with the specified ID exists in the same room?
        bool lua_hasSiblingFacility(const std::string& facilityID) const;

        //! If the facility with the specified exists in the same room we are in, returns its UID.
        //! If not, returns an unknown value.
        uint32 lua_getSiblingFacility(const std::string& facilityID) const;

        //! Get the current room x coordinate.
        uint32 lua_getCurrentRoomX() const;

        //! Get the current room y coordinate.
        uint32 lua_getCurrentRoomY() const;

        //----- Tresure

        //! Whether the facility currently hold treasure.
        bool lua_hasTreasure() const;

        //! Set the treasure held by the facility.
        void lua_setTreasure(const uint32 value);

        //----- Implicit link

        //! Get the state of this facility: are we created as a link?
        bool lua_isLink();

        //----- Link

        //! Has the facility an absolute link to an other room?
        bool lua_linkExists() const;

        //! The x coordinate of the absolute link. (Return -1u if none.)
        uint32 lua_linkGetX() const;

        //! The y coordinate of the absolute link. (Return -1u if none.)
        uint32 lua_linkGetY() const;

        //----- Energy

        //! Send a plus of enerdy to the specified room.
        void lua_energySendPulseRoom(const uint32 x, const uint32 y);

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

        //----- Lighting

        //! Add a lightPoint in position (x, y) and scale s.
        uint32 lua_lightAddPoint(lua_Number x, lua_Number y, lua_Number s);

        //! Sets the color of a previously added light.
        void lua_lightSetColor(uint32 lightID, uint r, uint g, uint b);

        //! @}

    private:

        RoomCoords m_coords;                    //!< The room in which the facility is set.
        FacilityInfo* m_facilityInfo = nullptr; //!< The facility data reference.
        std::wstring m_elementID;               //!< The last facility ID known.
    };
}
