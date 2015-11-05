#include "spriter/sfmlimagefile.hpp"

#include "core/application.hpp"

#include <Spriter/objectinfo/universalobjectinterface.h>

#include <iostream>

namespace SpriterEngine
{

    SfmlImageFile::SfmlImageFile(std::string initialFilePath, point initialDefaultPivot, sf::RenderWindow *validRenderWindow) :
        ImageFile(initialFilePath,initialDefaultPivot),
        renderWindow(validRenderWindow)
    {
        initializeFile();
    }

    void SfmlImageFile::initializeFile()
    {
        std::string fileID = Application::context().textures.getID(path());
        auto& texture = Application::context().textures.get(fileID);
        sprite.setTexture(texture);
        m_textureSize = texture.getSize();
    }

    void SfmlImageFile::renderSprite(UniversalObjectInterface * spriteInfo)
    {
        /*auto tiltColor = spriteInfo->getTiltColor();*/
        sf::Color tiltColor = sf::Color::White;
        tiltColor.a *= spriteInfo->getAlpha();

        sprite.setColor(tiltColor);
        sprite.setPosition(spriteInfo->getPosition().x, spriteInfo->getPosition().y);
        sprite.setRotation(toDegrees(spriteInfo->getAngle()));
        sprite.setScale(spriteInfo->getScale().x, spriteInfo->getScale().y);
        sprite.setOrigin(spriteInfo->getPivot().x*m_textureSize.x, spriteInfo->getPivot().y*m_textureSize.y);
        renderWindow->draw(sprite);
    }

}
