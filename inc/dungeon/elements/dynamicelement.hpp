#pragma once

#include "dungeon/elements/element.hpp"

namespace dungeon
{
    //! A generic dynamic element interface.
    //! A dynamic element is affected by gravity, but does not move by itself.

    class DynamicElement : public Element
    {
        using baseClass = Element;

    public:

        //! Constructor.
        DynamicElement(Inter& inter);

        //! Default destructor.
        virtual ~DynamicElement() = default;

    protected:

        //----------------//
        //! @name Lua API
        //! @{

        //! Get the current room x coordinate.
        uint lua_getCurrentRoomX() const;

        //! Get the current room y coordinate.
        uint lua_getCurrentRoomY() const;

        //! @}
    };
}
