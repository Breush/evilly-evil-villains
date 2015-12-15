#include "dungeon/detector.hpp"

#include "dungeon/detectentity.hpp"
#include "tools/platform-fixes.hpp" // std::erase_if
#include "tools/vector.hpp"
#include "tools/tools.hpp"

using namespace dungeon;

//----------------------------//
//----- Static variables -----//

Detector dungeon::s_detector;

//-----------------------//
//----- Registering -----//

void Detector::addEntity(DetectEntity& entity)
{
    m_entities.emplace_back(&entity);
    entity.setUID(m_UIDGenerator++);
}

void Detector::removeEntity(DetectEntity& entity)
{
    std::erase_if(m_entities, [&entity] (const DetectEntity* pEntity) { return pEntity == &entity; } );
}

//---------------------//
//----- Detection -----//

DetectEntity* Detector::find(const uint32 UID)
{
    for (auto& pEntity : m_entities)
        if (pEntity->UID() == UID)
            return pEntity;

    // Not found
    return nullptr;
}

const DetectEntity* Detector::find(const uint32 UID) const
{
    for (const auto& pEntity : m_entities)
        if (pEntity->UID() == UID)
            return pEntity;

    // Not found
    return nullptr;
}

uint32 Detector::isInRange(const DetectEntity& entity, const std::string& key, const float range) const
{
    // Range squared
    const auto sqRange = range * range;

    // Check if any in range for all corresponding key
    const auto& position = entity.localPosition();
    for (const auto& pEntity : m_entities) {
        if (!pEntity->detectVisible()) continue;
        if (pEntity->detectKey() != key || pEntity == &entity) continue;

        const auto distance = position - pEntity->localPosition();
        const auto sqDistance = distance.x * distance.x + distance.y * distance.y;
        if (sqDistance <= sqRange)
            return pEntity->UID();
    }

    return -1u;
}

void Detector::applyInRange(const sf::Vector2f& position, float range, DetectionLambda rangeEntityFunc)
{
    // Range squared
    const auto sqRange = range * range;

    // Check if any in range
    for (const auto& pEntity : m_entities) {
        if (!pEntity->detectVisible()) continue;

        const auto distance = position - pEntity->localPosition();
        const auto sqDistance = distance.x * distance.x + distance.y * distance.y;
        if (sqDistance <= sqRange)
            rangeEntityFunc(*pEntity, std::sqrt(sqDistance));
    }
}
