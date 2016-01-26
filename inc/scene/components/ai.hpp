#pragma once

#include "scene/components/component.hpp"

#include <SFML/Window/Event.hpp>

#include <selene/selene.hpp>

namespace scene
{
    //! Allows to execute a LUA script.

    class AI final : public Component
    {
        using baseClass = Component;

    public:

        //! Constructor.
        AI(Entity& entity);

        //! Default destructor.
        ~AI() = default;

        static std::string id() noexcept { return "AI"; }

        //------------------//
        //! @name Lua state
        //! @{

        //! Access the lua state.
        sel::State& luaState() { return m_lua; }

        //! @}

    private:

        sel::State m_lua;   //!< The lua state.
    };
}

