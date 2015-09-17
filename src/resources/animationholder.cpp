#include "resources/animationholder.hpp"

#include "scml/data.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "tools/platform-fixes.hpp"

using namespace resources;

//-------------------//
//----- Routine -----//

void AnimationHolder::update(const sf::Time& dt)
{
    for (auto& animatedSprite : m_animatedSprites)
        animatedSprite->updateAnimation(dt);
}

//-------------------//
//----- Storage -----//

void AnimationHolder::load(const std::string& filename)
{
    // Data
    m_scmlHolder.load(filename);
    auto id = m_scmlHolder.getID(filename);

    // File system
    auto fs = std::make_unique<scml::FileSystem>();
    m_fsMap.insert(std::make_pair(id, std::move(fs)));
    m_fsMap[id]->load(&getData(id));
}

//-------------------//
//----- Getters -----//

scml::Data& AnimationHolder::getData(const std::string& id)
{
    return m_scmlHolder.get(id);
}

scml::FileSystem& AnimationHolder::getFileSystem(const std::string& id)
{
    return (m_scmlHolder.stored(id))? *m_fsMap[id] : *m_fsMap["default"];
}

//----------------------------//
//----- Animated sprites -----//

void AnimationHolder::push(sfe::AnimatedSprite* animatedSprite)
{
    m_animatedSprites.emplace_back(animatedSprite);
}

void AnimationHolder::pop(sfe::AnimatedSprite* animatedSprite)
{
    auto found = std::find(m_animatedSprites.begin(), m_animatedSprites.end(), animatedSprite);
    m_animatedSprites.erase(found);
}
