#pragma once

#include "context/event.hpp"
#include "dungeon/elementdata.hpp"
#include "dungeon/detectentity.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"

#include <selene/selene.hpp>

// TODO Refactor some function with dungeon::Monster

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! A generic trap interface.

    class Trap final : public DetectEntity, public context::EventReceiver
    {
        using baseClass = DetectEntity;

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

        //! Indicate that harvestable dosh has changed.
        void lua_warnHarvestableDosh();

        //! Init the eData with the value if empty.
        void lua_initEmptyDataU32(const std::string& s, const uint32 value);

        //! Set the eData with the value specified.
        void lua_setDataU32(const std::string& s, const uint32 value);

        //! Set the eData with the value specified.
        void lua_setUIDDataU32(const uint32 UID, const std::string& s, const uint32 value);

        //! Get the correponding eData.
        uint32 lua_getDataU32(const std::string& s) const;

        //! Get the corresponding eData from UID.
        uint32 lua_getUIDDataU32(const uint32 UID, const std::string& s) const;

        //! Select an animation to play.
        void lua_selectAnimation(const std::string& animationKey);

        //! Returns true if the current animation has stopped (looping has to be false).
        bool lua_isAnimationStopped() const;

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! @}

    protected:

        Inter& m_inter;         //!< To be able to interact with nearby elements.
        ElementData& m_edata;   //!< The data corresponding to the monster.
        sf::Vector2u m_coords;  //!< The room in which the trap is set.

        // Artificial Intelligence
        sel::State m_lua;   //!< The lua state.

        // Decorum
        scene::AnimatedSprite m_sprite;   //!< The sprite.
    };
}
