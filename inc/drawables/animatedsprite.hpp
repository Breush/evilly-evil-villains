#pragma once

#include "tools/param.hpp"
#include "scml/interface.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <memory> // unique_ptr
#include <list>

namespace sfe
{

    class AnimatedSprite : public sf::Drawable, public sf::Transformable
    {
    public:
        AnimatedSprite();
        virtual ~AnimatedSprite();

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Updates
        void refresh();
        void update(const sf::Time& dt);

        // Controls animation
        void load(Animations::ID id, int number = 0);
        void clear();

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
