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
        Trap(const Data::Room& room) : m_dosh(0u), m_room(&room) {}

        //! Default destructor.
        virtual ~Trap() = default;

        //------------------//
        //! @name Resources
        //! @{

        //! Add a certain amount of dosh to current resources.
        inline void addDosh(uint amount) { setDosh(m_dosh + amount); }

        //! Substract a certain amount of dosh to current resources.
        inline void subDosh(uint amount) { setDosh(m_dosh - amount); }

        //! How much dosh is harvestable from this trap.
        virtual uint harvestableDosh() const = 0;

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //!< How much dosh the trap holds currently.
        PARAMGSU(uint, m_dosh, dosh, setDosh, changedDosh);

        //! @}

    protected:

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Whenever the dosh changed.
        virtual inline void changedDosh() {}

        //! @}

    protected:

        const Data::Room* m_room;   //!< The room in which the trap is set.
    };
}
