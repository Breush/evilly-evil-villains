#pragma once

#include "scene/entity.hpp"
#include "tools/param.hpp"
#include "scml/interface.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/Time.hpp>
#include <memory> // unique_ptr
#include <list>

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
        void load(Animations::ID id, int number = 0);
        void restart();

    protected:
        // Params
        PARAMG(bool, m_started, started)
        PARAMGSU(bool, m_looping, looping, setLooping, refresh)

    private:
        std::list<std::unique_ptr<scml::Entity>> m_entities;
        int m_number;
    };
}
