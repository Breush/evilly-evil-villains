#pragma once

#include "scene/entity.hpp"
#include "dungeon/graph.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! A hero invading the dungeon.

    class Hero final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Hero(const Inter* inter);

        //! Default destructor.
        ~Hero() = default;

        //----------------------//
        //! @name Dungeon graph
        //! @{

        //! The graph of the dungeon to be read from.
        void useGraph(const Graph& data);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recompute the local position of the hero.
        //! @todo This function should not exists once it is managed by physics component.
        void refreshPositionFromNode();

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt) final;

        //! @}

    private:

        //! The graph of the dungeon to be read from.
        const Graph* m_graph = nullptr;

        //! The dungeon inter, to get cellsize.
        //! @todo Should not be needed, only the abstract graph.
        const Inter* m_inter = nullptr;

        //! The current room where is our hero.
        const Graph::Node* m_currentNode = nullptr;

        //! How many seconds the hero is in the current room.
        float m_inRoomSince = 0.f;

        //! The sprite of the hero.
        sf::RectangleShape m_sprite;
    };
}
