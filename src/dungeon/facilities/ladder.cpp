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
    m_sprite.centerOrigin();
    setDesign(Design::MAIN);
}

//------------------//
//----- Design -----//

void Ladder::setDesign(Design design)
{
    switch (design) {
    case Design::MAIN:
        m_sprite.setTexture(TextureID::DUNGEON_FACILITIES_LADDER_MAIN);
        break;

    case Design::EXIT_MAIN:
        m_sprite.setTexture(TextureID::DUNGEON_FACILITIES_LADDER_EXIT_MAIN);
        break;

    case Design::EXIT_END:
        m_sprite.setTexture(TextureID::DUNGEON_FACILITIES_LADDER_EXIT_END);
        break;
    }
}
