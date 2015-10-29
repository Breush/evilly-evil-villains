#pragma once

#include "scene/entity.hpp"
#include "context/event.hpp"
#include "dungeon/elementdata.hpp"
#include "tools/param.hpp"

namespace dungeon
{
    // Forward declarations

    class Inter;
    class Graph;

    //! A generic monster interface.
    class Monster : public scene::Entity, public context::EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Monster(sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
            : baseClass(true)
            , m_active(false)
            , m_coords(coords)
            , m_inter(inter)
            , m_elementdata(elementdata)
        {
            setDetectable(false);
        }

        //! Default destructor.
        virtual ~Monster() = default;

        virtual std::string _name() const override = 0;

        //--------------------------//
        //! @name Public properties
        //! @{

        //! To be set to false when monster should stop.
        PARAMGSU(bool, m_active, active, setActive, refreshFromActivity)

        //! @}

        //--------------//
        //! @name Graph
        //! @{

        //! The graph of the dungeon to be read from.
        virtual void useGraph(Graph& graph) = 0;

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        virtual void receive(const context::Event& event) override = 0;

        //! @}

        //----------------//
        //! @name Events
        //! @{

        //! Detect if any hero is nearby this entity.
        //! @param relRange is expressed relatively to room size.
        bool isHeroNearby(float relRange) const;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Activity status changed.
        virtual void refreshFromActivity() = 0;

        //! @}

    protected:

        sf::Vector2u& m_coords;         //!< The room in which the monster is set.
        dungeon::Inter& m_inter;        //!< To be able to interact with nearby elements.
        ElementData& m_elementdata;     //!< The data corresponding to the monster.
    };
}
