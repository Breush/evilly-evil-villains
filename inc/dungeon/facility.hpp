#pragma once

#include "dungeon/elementdata.hpp"
#include "dungeon/detectentity.hpp"
#include "scene/wrappers/animatedsprite.hpp"

#include <selene/selene.hpp>

namespace dungeon
{
    // Forward declarations

    class FacilityInfo;
    class Inter;

    //! A generic facility interface.

    class Facility final : public DetectEntity
    {
        using baseClass = DetectEntity;

    public:

        //! Constructor.
        Facility(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter);

        //! Default destructor.
        virtual ~Facility() = default;

        std::string detectKey() const final { return "facility"; }
        std::string _name() const final { return "dungeon::Facility"; }

    protected:

        //----------------//
        //! @name Lua API
        //! @{

        //! Calling detector.
        void lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition);

        //! Set the depth of the entity.
        void lua_setDepth(const lua_Number inDepth);

        //! Get the state of this facility: are we created as a link?
        bool lua_isLink();

        //! Get the n-th relative tunnel from the list.
        //! Return 0u if not found.
        uint32 lua_getRtunnel(const uint32 nth) const;

        //! Add a relative tunnel to the list.
        void lua_addRtunnel(const uint32 direction);

        //! Checks if a tunnel exists.
        bool lua_hasTunnel() const;

        //! Add a tunnel.
        void lua_addTunnel(const uint32 x, const uint32 y);

        //! Remove all tunnels.
        void lua_removeTunnels();

        //! Select an animation to play.
        void lua_selectAnimation(const std::string& animationKey);

        //! Does a facility with the specified ID exists in the same room?
        bool lua_hasSiblingFacility(const std::string& facilityID) const;

        //! Control the visibility of the entity.
        void lua_setVisible(bool isVisible);

        //! Has the facility an absolute link to an other room?
        bool lua_hasLink() const;

        //! The x coordinate of the absolute link. (Return -1u if none.)
        uint32 lua_getLinkRoomX() const;

        //! The x coordinate of the absolute link. (Return -1u if none.)
        uint32 lua_getLinkRoomY() const;

        //! Get the current room x coordinate.
        uint32 lua_getCurrentRoomX() const;

        //! Get the current room y coordinate.
        uint32 lua_getCurrentRoomY() const;

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! @}

    private:

        sf::Vector2u m_coords;          //!< The room in which the facility is set.
        dungeon::Inter& m_inter;        //!< To be able to interact with nearby elements.
        FacilityInfo& m_facilityInfo;   //!< The facility data reference.
        scene::AnimatedSprite m_sprite; //!< The sprite.
        sel::State m_lua;               //!< The lua state.
    };
}
