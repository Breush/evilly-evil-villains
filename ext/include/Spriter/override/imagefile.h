#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include <string>

#include "../file/file.h"
#include "../global/global.h"

#include <SFML/Graphics/RenderTarget.hpp>

namespace SpriterEngine
{
    class UniversalObjectInterface;

    class ImageFile : public File
    {
    public:
        ImageFile(std::string initialFilePath, point initialDefaultPivot);

        ImageFile *imageFile() override;

        point defaultPivot;

        virtual void renderSprite(UniversalObjectInterface *spriteInfo, sf::RenderTarget& target, sf::RenderStates& states, const sf::Color& tiltColor) = 0;
    };

}

#endif // IMAGEFILE_H
