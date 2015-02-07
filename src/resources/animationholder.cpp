#include "resources/animationholder.hpp"

#include "scml/interface.hpp"

AnimationHolder::AnimationHolder()
{
}

void AnimationHolder::update(const sf::Time& dt)
{
    for (auto& animatedSprite : m_animatedSprites)
        animatedSprite->update(dt);
}

void AnimationHolder::load(Animations::ID id, const std::string& filename)
{
    // Data
    m_scmlHolder.load(id, filename);

    // File system
    std::unique_ptr<scml::FileSystem> fs(new scml::FileSystem());
    m_fsMap.insert(std::make_pair(id, std::move(fs)));
    m_fsMap[id]->load(&getData(id));
}

//----- Getters -----//

scml::Data& AnimationHolder::getData(Animations::ID id)
{
    return m_scmlHolder.get(id);
}

scml::FileSystem& AnimationHolder::getFileSystem(Animations::ID id)
{
    return *m_fsMap[id];
}

//----- Animated sprites -----//

void AnimationHolder::push(sfe::AnimatedSprite* animatedSprite)
{
    m_animatedSprites.push_back(animatedSprite);
}

void AnimationHolder::pop(sfe::AnimatedSprite* animatedSprite)
{
    auto found = std::find(m_animatedSprites.begin(), m_animatedSprites.end(), animatedSprite);
    m_animatedSprites.erase(found);
}