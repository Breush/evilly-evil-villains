#include "dungeon/hero.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "tools/debug.hpp"

using namespace dungeon;

Hero::Hero(const Inter* inter)
    : m_inter(inter)
{
    // Sprite
    m_sprite.setSize({10.f, 10.f});
    m_sprite.setFillColor(sf::Color::White);
    addPart(&m_sprite);
}

//-------------------//
//----- Routine -----//

void Hero::updateAI(const sf::Time& dt)
{
    m_inRoomSince += dt.asSeconds();

    // TODO Currently it's using raw data, we should create an abstract graph of the dungeon
    // TODO Interface with physics engine

    // Look for next room each two seconds
    if (m_inRoomSince >= 2.f) {
        m_inRoomSince -= 2.f;

        // TODO Not managing presence of ladder...

        sf::Vector2u offset(1u, 0u);
        while (!m_data->isRoomConstructed(m_currentRoom + offset)) {
            offset.x = rand() % 3u - 1u;
            offset.y = 0u;

            if (rand() % 2u == 0u)
                std::swap(offset.x, offset.y);
        }
        m_currentRoom += offset;

        refreshPositionFromRoom();
    }
}

//------------------------//
//----- Dungeon data -----//

void Hero::useData(Data& data)
{
    m_data = &data;
}

//-----------------------------------//
//----- Internal change updates -----//

void Hero::refreshPositionFromRoom()
{
    setLocalPosition(m_inter->roomLocalPosition(m_currentRoom) + m_inter->roomSize() / 2.f);
}
