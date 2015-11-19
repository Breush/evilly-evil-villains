#include "dungeon/elements/monster.hpp"

using namespace dungeon;

Monster::Monster(Inter& inter, Graph& graph)
    : baseClass("dungeon/monsters/", inter, graph)
{
}
