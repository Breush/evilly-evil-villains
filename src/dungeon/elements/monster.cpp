#include "dungeon/elements/monster.hpp"

#include "scene/components/lerpable.hpp"
#include "dungeon/inter.hpp"

using namespace dungeon;

Monster::Monster(Inter& inter, Graph& graph)
    : baseClass("vanilla/monsters/", inter, graph)
{
}

//-------------------//
//----- Control -----//

void Monster::setDamaged(bool damaged)
{
    // TODO we should probably select an animation
    m_sprite.setTiltColor(damaged? sf::Color::Red : sf::Color::White);
}

//------------------------//
//----- Element data -----//

void Monster::rebindElementData()
{
    // Update aspect
    setLocalScale(m_inter.roomScale());
    setSize(m_inter.tileSize());

    // Update from data
    const auto& monsterData = m_inter.monstersDB().get(m_elementID);
    getComponent<scene::Lerpable>()->setPositionSpeed(m_inter.tileSize() * monsterData.speed);
    m_pauseDelay = monsterData.pauseDelay;
}
