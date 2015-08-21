#pragma once

#include "scml/interface.hpp"
#include "resources/holder.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <list>
#include <map>

// TODO DOC

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

        void load(const std::string& filename);
        void update(const sf::Time& dt);

        // Called by animated sprites
        void push(sfe::AnimatedSprite* animatedSprite);
        void pop(sfe::AnimatedSprite* animatedSprite);

        // Getters
        scml::Data& getData(const std::string& id);
        scml::FileSystem& getFileSystem(const std::string& id);


    private:
        SCMLHolder m_scmlHolder;
        std::map<std::string, std::unique_ptr<scml::FileSystem>> m_fsMap;
        std::list<sfe::AnimatedSprite*> m_animatedSprites;
    };
}
