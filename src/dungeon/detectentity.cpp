#include "dungeon/detectentity.hpp"

#include "dungeon/detector.hpp"
#include "tools/string.hpp"

#include <iostream>

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

//------------------//
//---- Routine -----//

void DetectEntity::update(const sf::Time& dt)
{
    uint32 UID;

    // Run callbacks which condition are met
    if (m_detectActive)
        for (const auto& signal : m_detectSignals)
            if ((UID = signal.condition()) != -1u)
                signal.callback(UID);

    baseClass::update(dt);
}

//--------------------//
//---- Detection -----//

DetectEntity::DetectCondition DetectEntity::interpretDetectCondition(const std::string& key, const std::string &condition)
{
    // TODO Make a correct parser out of these
    auto tokens = split(condition);
    if (tokens.size() >= 3u) {
        // In range
        if (tokens[0u] == "distance" && tokens[1u] == "<") {
            std::stringstream str;
            str << tokens[2u];
            float range;
            str >> range;
            return [this, key, range] { return isInRange(key, range); };
        }
    }

    std::cerr << "ERROR: Cannot interpret condition '" << condition << "'." << std::endl;
    return [] { return -1u; };
}

void DetectEntity::addDetectSignal(const std::string& key, const std::string& condition, DetectCallback callback)
{
    DetectSignal signal;
    signal.condition = interpretDetectCondition(key, condition);
    signal.callback = std::move(callback);
    m_detectSignals.emplace_back(std::move(signal));
}

void DetectEntity::removeDetectSignals()
{
    m_detectSignals.clear();
}

uint32 DetectEntity::isInRange(const std::string& key, float range) const
{
    return s_detector.isInRange(*this, key, m_detectRangeFactor * range);
}
