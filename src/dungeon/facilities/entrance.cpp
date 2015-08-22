#include "dungeon/facilities/entrance.hpp"

#include "tools/tools.hpp"

using namespace dungeon::facilities;

Entrance::Entrance(const sf::Vector2u& coords, ElementData& elementdata)
    : baseClass(coords, elementdata)
{
    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/facilities/entrance");
}
