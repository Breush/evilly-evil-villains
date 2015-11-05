#pragma once

#include <Spriter/objectinfo/boxinstanceinfo.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace SpriterEngine
{

    class SfmlBoxInstanceInfo : public BoxInstanceInfo
    {
    public:
        SfmlBoxInstanceInfo(point initialSize, sf::RenderWindow *validRenderWindow);

        void render() override;

    private:
        sf::RenderWindow *renderWindow;

        sf::RectangleShape rectangle;
    };

}
