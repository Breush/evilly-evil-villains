#pragma once

#include <Spriter/objectinfo/boxinstanceinfo.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace SpriterEngine
{

    class SfmlBoxInstanceInfo final : public BoxInstanceInfo
    {
    public:

        //! Constructor.
        SfmlBoxInstanceInfo(point initialSize);

        void render(sf::RenderTarget& target, sf::RenderStates& states, const sf::Color& tiltColor) final;

    private:
        sf::RenderWindow *renderWindow;

        sf::RectangleShape rectangle;
    };

}
