#pragma once

#include "scene/entity.hpp"
#include "dungeon/elementdata.hpp"
#include "dungeon/event.hpp"

namespace dungeon
{
    //! A generic monster interface.
    class Monster : public scene::Entity, public EventReceiver
    {
    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Monster(const sf::Vector2u& coords, ElementData& elementdata)
            : m_coords(coords)
            , m_elementdata(elementdata)
        {
            setDetectable(false);
        }

        //! Default destructor.
        virtual ~Monster() = default;

    protected:

        //---------------//
        //! @name Events
        //! @{

        virtual void receive(const Event& event) override = 0;

        //! @}

    protected:

        sf::Vector2u m_coords;          //!< The initial room in which the monster is set.
        ElementData& m_elementdata;     //!< The data corresponding to the monster.
    };
}
