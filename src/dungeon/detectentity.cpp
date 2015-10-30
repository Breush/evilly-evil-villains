#include "dungeon/detectentity.hpp"

#include "dungeon/detector.hpp"

using namespace dungeon;

DetectEntity::DetectEntity(bool isLerpable)
    : baseClass(isLerpable)
{
    s_detector.addEntity(*this);
}

DetectEntity::~DetectEntity()
{
    s_detector.removeEntity(*this);
}

//-------------------//
//---- Detector -----//

bool DetectEntity::isInRange(const std::string& key, float range) const
{
    return s_detector.isInRange(*this, key, range);
}
