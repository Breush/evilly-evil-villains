#include "resources/animationholder.hpp"

#include "core/application.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "tools/platform-fixes.hpp"

#include <iostream>

using namespace resources;

AnimationHolder::AnimationHolder()
{
}

//-------------------//
//----- Storage -----//

std::string AnimationHolder::getID(const std::string& filename)
{
    std::string id = filename.substr(0u, filename.find_last_of("."));
    return id.substr(id.find_first_of("/") + 1u);
}

void AnimationHolder::load(const std::string& filename)
{
    auto newFileFactory = new SpriterEngine::SpriterFileFactory();
    auto newObjectFactory = new SpriterEngine::SpriterObjectFactory();

    auto id = getID(filename);
    auto model = std::make_unique<SpriterEngine::SpriterModel>(filename, newFileFactory, newObjectFactory);
    m_models.emplace(id, std::move(model));
}

void AnimationHolder::free(const std::string& id)
{
    m_models.erase(id);
}

void AnimationHolder::freeMatchingPrefix(const std::string& prefix)
{
    // Find matching IDs
    std::vector<std::string> matchingIDs;
    for (const auto& model : m_models)
        if (std::mismatch(std::begin(prefix), std::end(prefix), std::begin(model.first)).first == std::end(prefix))
            matchingIDs.emplace_back(model.first);

    // Remove them
    for (const auto& id : matchingIDs)
        free(id);
}

//-------------------------------//
//----- Spriter interfacing -----//

SpriterEngine::SpriterModel& AnimationHolder::getModel(const std::string& id)
{
    auto found = m_models.find(id);

    // If ID doesn't exist, try to use the default back-up
    if (found == std::end(m_models)) {
        found = m_models.find(m_defaultID);
        if (found == std::end(m_models))
            throw std::runtime_error("Animation id '" + id + "' was not found and there's no default back-up. Ouch.");
    }
    return *found->second;
}
