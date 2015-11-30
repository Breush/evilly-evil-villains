#pragma once

#include "dungeon/elements/movingelement.hpp"

namespace dungeon
{
    // Forward declarations

    class HeroesManager;

    //! A hero invading the dungeon.

    class Hero final : public MovingElement
    {
        using baseClass = MovingElement;

    public:

        //! Constructor.
        Hero(HeroesManager& manager, Inter& inter, Graph& graph);

        //! Default destructor.
        ~Hero() = default;

        std::string detectKey() const final { return "hero"; }
        std::string _name() const final { return "dungeon::Hero"; }

        //-------------------//
        //! @name Extern lua
        //! @{

        //! Calls the Lua _onDeath() function.
        void onDeath();

        //! @}

    protected:

        //---------------------//
        //! @name Element data
        //! @{

        void rebindElementData() final;

        //! @}

        //----------------//
        //! @name Lua API
        //! @{

        //! When the hero wants to get out.
        void lua_getOut();

        //! When the hero wants to steal money from treasure.
        uint lua_stealTreasure();

        //! @}

    private:

        HeroesManager& m_manager;   //!< The heroes manager.
    };
}
