#pragma once

#include "scml/interface.hpp"
#include "resources/holder.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <list>
#include <map>

// Forward declarations

namespace sfe
{
    class AnimatedSprite;
}

namespace scml
{
    class Data;
}

namespace resources
{
    class AnimationHolder final : private sf::NonCopyable
    {
    public:
        AnimationHolder() = default;

        void load(AnimationID id, const std::string& filename);
        void update(const sf::Time& dt);

        // Called by animated sprites
        void push(sfe::AnimatedSprite* animatedSprite);
        void pop(sfe::AnimatedSprite* animatedSprite);

        // Getters
        scml::Data& getData(AnimationID id);
        scml::FileSystem& getFileSystem(AnimationID id);


    private:
        SCMLHolder m_scmlHolder;
        std::map<AnimationID, std::unique_ptr<scml::FileSystem>> m_fsMap;
        std::list<sfe::AnimatedSprite*> m_animatedSprites;
    };
}
