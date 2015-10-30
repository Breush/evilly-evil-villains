#include "dungeon/detector.hpp"

#include "dungeon/detectentity.hpp"
#include "tools/platform-fixes.hpp" // std::erase_if
#include "tools/vector.hpp"
#include "tools/tools.hpp"

using namespace dungeon;

//----------------------------//
//----- Static variables -----//

// TODO This can cause issues with update(dt).
// How can we be sure its run only once per frame?
Detector dungeon::s_detector;

//-------------------//
//----- Routine -----//

void Detector::update(const sf::Time& dt)
{
}

//-----------------------//
//----- Registering -----//

void Detector::addEntity(DetectEntity& entity)
{
    m_entities.emplace_back(&entity);
}

void Detector::removeEntity(DetectEntity& entity)
{
    std::erase_if(m_entities, [&entity] (const DetectEntity* pEntity) { return pEntity == &entity; } );
}

//---------------------//
//----- Detection -----//

bool Detector::isInRange(const DetectEntity& entity, const std::string& key, const float range) const
{
    // Range squared
    const auto sqRange = range * range;

    // Check if any in range for all corresponding key
    const auto& position = entity.localPosition();
    for (const auto& pEntity : m_entities) {
        if (pEntity->detectKey() != key) continue;

        const auto distance = position - pEntity->localPosition();
        const auto sqDistance = distance * distance;
        if ((sqDistance.x + sqDistance.y) < sqRange)
            return true;
    }

    return false;
}
