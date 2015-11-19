#pragma once

#include "dungeon/elements/elementdata.hpp"
#include "dungeon/detectentity.hpp"
#include "scene/wrappers/animatedsprite.hpp"

#include <selene/selene.hpp>

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! A generic interface for elements (facility, trap, monster).
    //! Holds the common parts of the lua API.

    class Element : public DetectEntity
    {
        using baseClass = DetectEntity;

    public:

        //! Constructor.
        Element(dungeon::Inter& inter, bool isLerpable = false);

        //! Default destructor.
        virtual ~Element() = default;

    protected:

        //----------------//
        //! @name Lua API
        //! @{

        //! Calling detector.
        void lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition);

        //! Set the depth of the entity.
        void lua_setDepth(const lua_Number inDepth);

        //! Control the visibility of the entity.
        void lua_setVisible(bool isVisible);

        //----- Element data

        //! Set the eData with the value specified.
        void lua_setDataU32(const std::string& s, const uint32 value);

        //! Get the correponding eData.
        uint32 lua_getDataU32(const std::string& s) const;

        //! Init the eData with the value if empty.
        void lua_initEmptyDataU32(const std::string& s, const uint32 value);

        //! Set the eData with the value specified.
        void lua_setDataFloat(const std::string& s, const lua_Number value);

        //! Get the correponding eData.
        lua_Number lua_getDataFloat(const std::string& s) const;

        //! Init the eData with the value if empty.
        void lua_initEmptyDataFloat(const std::string& s, const lua_Number value);

        //----- Element data from UID

        //! Set the eData with the value specified.
        void lua_setUIDDataU32(const uint32 UID, const std::string& s, const uint32 value);

        //! Get the corresponding eData from UID.
        uint32 lua_getUIDDataU32(const uint32 UID, const std::string& s) const;

        //----- Animation

        //! Select an animation to play.
        void lua_selectAnimation(const std::string& animationKey);

        //! Returns true if the current animation has stopped (looping has to be false).
        bool lua_isAnimationStopped() const;

        //! Advance the current animation from a certain offset.
        void lua_forwardAnimation(const lua_Number offset);

        //! Restart the current animation from the beginning.
        void lua_restartAnimation();

        //----- Villain

        //! Borrow money from the reserve.
        //! Return the amount borrowed.
        uint32 lua_borrowVillainDosh(const uint32 amount);

        //----- Dungeon

        //! Explode the corresponding room.
        void lua_dungeonExplodeRoom(const uint x, const uint y);

        //----- Debug

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! @}

    protected:

        dungeon::Inter& m_inter;        //!< To be able to interact with nearby elements.
        ElementData* m_edata = nullptr; //!< The data of the element.
        scene::AnimatedSprite m_sprite; //!< The sprite.
        sel::State m_lua;               //!< The lua state.
    };
}
