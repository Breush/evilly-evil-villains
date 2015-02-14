#pragma once

#include "scml/interface.hpp"
#include "resources/holder.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <list>
#include <map>

// Forward declarations
namespace sfe {
    class AnimatedSprite;
}

namespace scml {
    class Data;
}

class AnimationHolder : private sf::NonCopyable
{
public:
    AnimationHolder();

    void load(Animations::ID id, const std::string& filename);
    void update(const sf::Time& dt);

    // Called by animated sprites
    void push(sfe::AnimatedSprite* animatedSprite);
    void pop(sfe::AnimatedSprite* animatedSprite);

    // Getters
    scml::Data& getData(Animations::ID id);
    scml::FileSystem& getFileSystem(Animations::ID id);


private:
    SCMLHolder m_scmlHolder;
    std::map<Animations::ID, std::unique_ptr<scml::FileSystem>> m_fsMap;
    std::list<sfe::AnimatedSprite*> m_animatedSprites;
};

