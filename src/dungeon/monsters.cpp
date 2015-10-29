#include "dungeon/monsters.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon;

//-------------------//
//----- Monster -----//

bool Monster::isHeroNearby(float relRange) const
{
    return m_inter.isHeroNearby(localPosition(), relRange);
}
