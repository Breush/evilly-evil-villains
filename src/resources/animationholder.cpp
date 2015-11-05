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

void AnimationHolder::load(const std::string& filename)
{
    // Remove res/xxx/ and file extension
    std::string id = filename.substr(0u, filename.find_last_of("."));
    id = id.substr(id.find_first_of("/") + 1u);
    id = id.substr(id.find_first_of("/") + 1u);

    // FIXME Use the ones in the class...
    // FIXME They might not ask for object.
    auto& window = Application::context().window;
    auto model = std::make_unique<SpriterEngine::SpriterModel>(filename, new SpriterEngine::ExampleFileFactory(&window), new SpriterEngine::ExampleObjectFactory(&window));
    m_models.emplace(id, std::move(model));

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
    m_models.erase(id);
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

//-------------------------------//
//----- Spriter interfacing -----//

SpriterEngine::SpriterModel& AnimationHolder::getModel(const std::string& id)
{
    auto found = m_models.find(id);
    if (found == std::end(m_models)) {
        std::cerr << "Animation id '" << id << "' was not found, using default." << std::endl;
        return *m_models.at("default");
    }
    return *found->second;
}
