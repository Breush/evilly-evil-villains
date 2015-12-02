#pragma once

#include <Spriter/objectinfo/pointinstanceinfo.h>

#include <SFML/Graphics/CircleShape.hpp>

namespace SpriterEngine
{

    class SfmlPointInstanceInfo final : public PointInstanceInfo
    {
    public:

        //! Constructor.
        SfmlPointInstanceInfo();

        void render(sf::RenderTarget& target, sf::RenderStates& states, const sf::Color& tiltColor) final;

    private:

        sf::CircleShape circle;
    };

}
