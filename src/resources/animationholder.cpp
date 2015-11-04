#include "resources/animationholder.hpp"

#include "scene/wrappers/animatedsprite.hpp"
#include "tools/platform-fixes.hpp"

using namespace resources;

//-------------------//
//----- Storage -----//

void AnimationHolder::load(const std::string& filename)
{
    /*// Data
    m_scmlHolder.load(filename);
    auto id = m_scmlHolder.getID(filename);

    // File system
    auto fs = std::make_unique<scml::FileSystem>();
    m_fsMap.insert(std::make_pair(id, std::move(fs)));
    m_fsMap[id]->load(&getData(id));*/
}

void AnimationHolder::free(const std::string& id)
{
    /*
    m_scmlHolder.free(id);
    m_fsMap.erase(id);*/
}

void AnimationHolder::freeMatchingPrefix(const std::string& prefix)
{
    /*
    // Find matching IDs
    std::vector<std::string> matchingIDs;
    for (const auto& fs : m_fsMap)
        if (std::mismatch(std::begin(prefix), std::end(prefix), std::begin(fs.first)).first == std::end(prefix))
            matchingIDs.emplace_back(fs.first);

    // Remove them
    for (const auto& id : matchingIDs)
        free(id);*/
}
