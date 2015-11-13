#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/animatedsprite.hpp"

#include <selene/selene.hpp>

// FIXME Move all that to a generic Facility interface

namespace dungeon
{
    // Forward declarations

    class FacilityInfo;

namespace facilities
{
    //!< The ladder facility.

    class Ladder final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Constructor.
        Ladder(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter);

        //! Default destructor.
        ~Ladder() = default;

        std::string _name() const final { return "dungeon::facilities::Ladder"; }

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final {}

        //! @}

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

        //! Select an animation to play.
        void lua_selectAnimation(const std::string& animationKey);

        //! Does a facility with the specified ID exists in the same room?
        bool lua_hasSiblingFacility(const std::string& facilityID) const;

        //! Control the visibility of the entity.
        void lua_setVisible(bool isVisible);

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! @}

    private:

        FacilityInfo& m_facilityInfo;   //!< The facility data reference.
        scene::AnimatedSprite m_sprite; //!< The sprite.
        sel::State m_lua;               //!< The lua state.
    };
}
}
