#include "dungeon/facilities/stairs.hpp"

#include "tools/tools.hpp"

using namespace dungeon::facilities;

Stairs::Stairs(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
    : baseClass(coords, elementdata, inter)
{
    // Is behind almost everything, but still in front of dungeon back wall
    setDepth(99.f);

    // Decorum
    attachChild(m_sprite);
    m_sprite.centerOrigin();
    m_sprite.load("dungeon/facilities/stairs");
}

//------------------//
//----- Design -----//

void Stairs::setDesign(Design design)
{
    switch (design) {
    case Design::UP_LEFT:
        m_sprite.select(L"lup");
        break;

    case Design::UP_RIGHT:
        m_sprite.select(L"rup");
        break;

    case Design::DOWN_LEFT:
        m_sprite.select(L"ldown");
        break;

    case Design::DOWN_RIGHT:
        m_sprite.select(L"rdown");
        break;
    }
}
