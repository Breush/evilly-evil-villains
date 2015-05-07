#pragma once

#include "scene/entity.hpp"
#include "dungeon/data.hpp"
#include "tools/int.hpp"

namespace dungeon
{
    //! The traps.
    /*!
     *  A trap is an object/structure to act against an invader.
     *  There can be only one per dungeon room.
     */
    enum TrapID : uint8
    {
        NONE,       //!< No trap set.
        PICKPOCK,   //!< Steals a bit of money from an invader.
    };

    //! A generic trap interface.
    class Trap : public scene::Entity
    {
    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Trap(const Data::Room& room) : m_room(&room) {}

        //! Default destructor.
        virtual ~Trap() = default;

    protected:

        const Data::Room* m_room;
    };
}
