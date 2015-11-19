#include "dungeon/elements/monster.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon;

Monster::Monster(Inter& inter, Graph& graph)
    : baseClass("dungeon/monsters/", inter, graph)
{
}

//------------------------//
//----- Element data -----//

void Monster::rebindElementData()
{
    const auto& monsterData = m_inter.monstersDB().get(m_elementID);
    lerpable()->setPositionSpeed(m_inter.tileSize() * monsterData.speed);
    m_pauseDelay = monsterData.pauseDelay;
}
