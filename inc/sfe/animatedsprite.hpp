#pragma once

#include "scene/entity.hpp"
#include "scml/interface.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/System/Time.hpp>
#include <memory> // unique_ptr
#include <list>

// Forward declarations

enum class AnimationID : uint8;

namespace sfe
{
    class AnimatedSprite : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        AnimatedSprite();
        virtual ~AnimatedSprite();

        // Routine
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void updateAnimation(const sf::Time& dt);
        void refresh();

        // Controls animation
        void load(AnimationID id, int number = 0);
        void restart();

    protected:
        // Params
        PARAMGSU(bool, m_looping, looping, setLooping, refresh)
        PARAMG(bool, m_started, started)

    private:
        std::list<std::unique_ptr<scml::Entity>> m_entities;
        int m_number;
    };
}
