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
    // Run callbacks which condition are met
    for (const auto& signal : m_detectSignals)
        if (signal.condition())
            signal.callback();

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
            float range = std::stof(tokens[2u]);
            return [this, key, range] { return isInRange(key, range); };
        }
    }

    std::cerr << "ERROR: Cannot interpret condition '" << condition << "'." << std::endl;
    return [] { return false; };
}

void DetectEntity::addDetectSignal(const std::string& key, const std::string& condition, DetectCallback callback)
{
    DetectSignal signal;
    signal.condition = interpretDetectCondition(key, condition);
    signal.callback = std::move(callback);
    m_detectSignals.emplace_back(std::move(signal));
}

bool DetectEntity::isInRange(const std::string& key, float range) const
{
    return s_detector.isInRange(*this, key, m_detectRangeFactor * range);
}
