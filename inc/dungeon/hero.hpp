#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;
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

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recompute the local position of the hero, given the room.
        //! @todo This function should not exists once it is managed by physics component.
        void refreshPositionFromRoom();

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt) final;

        //! @}

    private:

        //! The data of the dungeon to be read from.
        //! @todo Should not be needed, only the abstract graph.
        Data* m_data = nullptr;

        //! The dungeon inter, to get cellsize.
        //! @todo Should not be needed, only the abstract graph.
        const Inter* m_inter = nullptr;

        //! The current room where is our hero.
        sf::Vector2u m_currentRoom = {0u, 0u};

        //! How many seconds the hero is in the current room.
        float m_inRoomSince = 0.f;

        //! The sprite of the hero.
        sf::RectangleShape m_sprite;
    };
}
