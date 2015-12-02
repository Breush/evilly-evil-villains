#pragma once

#include <Spriter/override/imagefile.h>

#include <SFML/Graphics/Sprite.hpp>

namespace SpriterEngine
{
    class SfmlImageFile final : public ImageFile
    {
        using baseClass = ImageFile;

    public:

        // Constructor
        SfmlImageFile(std::string filePath, point defaultPivot);

        //----------------//
        //! @name Routine
        //! @{

        void renderSprite(UniversalObjectInterface *spriteInfo, sf::RenderTarget& target, sf::RenderStates& states, const sf::Color& tiltColor) final;

        //! @}

    private:

        sf::Sprite sprite;              //!< The sprite indeed.
        sf::Vector2u m_textureSize;     //!< The texture size.
    };

}
