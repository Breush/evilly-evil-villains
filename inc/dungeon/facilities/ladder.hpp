#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/sprite.hpp"

#include <selene/selene.hpp>

// FIXME Move all that to a gneric Facility interface

namespace dungeon
{
namespace facilities
{
    //!< The ladder facility.

    class Ladder final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Defines in what mode the ladder is drawn.
        enum class Design
        {
            MAIN,
            EXIT_END,
            EXIT_MAIN,
        };

    public:

        //! Constructor.
        Ladder(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Ladder() = default;

        std::string _name() const final { return "dungeon::facilities::Ladder"; }

        //---------------//
        //! @name Design
        //! @{

        //! Defines in what mode the ladder is drawn.
        void setDesign(Design design);

        //! @}

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

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! @}

    private:

        scene::Sprite m_sprite; //!< The sprite.

        sel::State m_lua;       //!< The lua state.
    };
}
}
