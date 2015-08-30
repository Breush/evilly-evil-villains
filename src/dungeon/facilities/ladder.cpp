#include "dungeon/facilities/ladder.hpp"

#include "tools/tools.hpp"

using namespace dungeon::facilities;

Ladder::Ladder(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
    : baseClass(coords, elementdata, inter)
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
        m_sprite.setTexture("dungeon/facilities/ladder/main");
        break;

    case Design::EXIT_MAIN:
        m_sprite.setTexture("dungeon/facilities/ladder/exit_main");
        break;

    case Design::EXIT_END:
        m_sprite.setTexture("dungeon/facilities/ladder/exit_end");
        break;
    }
}
