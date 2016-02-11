#include "dungeon/detectentity.hpp"

#include "dungeon/detector.hpp"
#include "tools/string.hpp"

#include <iostream>

using namespace dungeon;

DetectEntity::DetectEntity()
{
    s_detector.addEntity(*this);
}

DetectEntity::~DetectEntity()
{
    s_detector.removeEntity(*this);
}

//------------------//
//---- Routine -----//

void DetectEntity::update(const sf::Time& dt, const float factor)
{
    // Run callbacks which condition are met
    if (m_detectActive)
        for (const auto& signal : m_detectSignals)
            for (auto UID : signal.condition())
                signal.callback(UID);

    baseClass::update(dt, factor);
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
            return [this, key, range] { return inRangeUIDs(key, range); };
        }
    }

    std::cerr << "[DetectEntity] ERROR: Cannot interpret condition '" << condition << "'." << std::endl;
    return [] { return std::vector<UID_t>(); };
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

std::vector<DetectEntity::UID_t> DetectEntity::inRangeUIDs(const std::string& key, float range) const
{
    return s_detector.inRangeUIDs(*this, key, m_detectRangeFactor * range);
}
