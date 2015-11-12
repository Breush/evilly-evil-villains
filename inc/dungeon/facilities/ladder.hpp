#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/animatedsprite.hpp"

#include <selene/selene.hpp>

// FIXME Move all that to a gneric Facility interface

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

        //! Get the n-th relative link from the list.
        //! Return 0u if not found.
        uint32 lua_getRlink(const uint32 nth) const;

        //! Add a relative link to the list.
        void lua_addRlink(const uint32 direction);

        //! Select an animation to play.
        void lua_selectAnimation(const std::string& animationKey);

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! @}

    private:

        scene::AnimatedSprite m_sprite; //!< The sprite.
        sel::State m_lua;               //!< The lua state.

        FacilityInfo& m_facilityInfo;   //!< The facility data reference.
    };
}
}
