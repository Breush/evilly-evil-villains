#include "scene/components/ai.hpp"

using namespace scene;

AI::AI(scene::Entity& entity)
    : baseClass(entity)
    , m_lua(true)
{
}
