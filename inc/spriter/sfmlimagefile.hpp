#pragma once

#include <Spriter/override/imagefile.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace SpriterEngine
{
    class SfmlImageFile : public ImageFile
    {
    public:

        SfmlImageFile(std::string initialFilePath, point initialDefaultPivot, sf::RenderWindow *validRenderWindow);

        void renderSprite(UniversalObjectInterface *spriteInfo) override;

    private:

        sf::Sprite sprite;
        sf::RenderWindow *renderWindow;
        sf::Vector2u m_textureSize;

        void initializeFile();
    };

}
