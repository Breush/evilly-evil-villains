#pragma once

#include <Spriter/objectinfo/pointobjectinfo.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>

namespace SpriterEngine
{
    class SfmlPointObjectInfo : public PointObjectInfo
    {
    public:

        SfmlPointObjectInfo(sf::RenderWindow *validRenderWindow);

        void render() override;

    private:

        sf::RenderWindow *renderWindow;

        sf::CircleShape circle;
    };

}
