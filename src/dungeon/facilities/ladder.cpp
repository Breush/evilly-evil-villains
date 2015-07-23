#include "dungeon/facilities/ladder.hpp"

#include "resources/identifiers.hpp"
#include "tools/tools.hpp"

using namespace dungeon::facilities;

Ladder::Ladder(const sf::Vector2u& coords, ElementData& elementdata)
    : baseClass(coords, elementdata)
{
    // Is behind almost everything
    setDepth(500.f);

    // Decorum
    attachChild(m_sprite);
    m_sprite.setTexture(TextureID::DUNGEON_FACILITIES_LADDER_MAIN);
    m_sprite.centerOrigin();
}
