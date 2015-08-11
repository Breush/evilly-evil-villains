#include "dungeon/monsters/creepim.hpp"

using namespace dungeon::monsters;

Creepim::Creepim(const sf::Vector2u& coords, ElementData& elementdata)
    : baseClass(coords, elementdata)
{
    // Decorum
    attachChild(m_sprite);
    m_sprite.setFillColor(sf::Color::White);
    m_sprite.setSize({50.f, 50.f});
}

//-------------------------//
//---- Dungeon events -----//

void Creepim::receive(const Event& event)
{
}

