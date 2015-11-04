#include "spriter/sfmlimagefile.hpp"

#include <Spriter/objectinfo/universalobjectinterface.h>

#include "tools/vector.hpp" // FIXME
#include <iostream> // FIXME

namespace SpriterEngine
{

    SfmlImageFile::SfmlImageFile(std::string initialFilePath, point initialDefaultPivot, sf::RenderWindow *validRenderWindow) :
        ImageFile(initialFilePath,initialDefaultPivot),
        renderWindow(validRenderWindow)
    {
        initializeFile();
    }

    // TODO Here, use Application::context().textures
    void SfmlImageFile::initializeFile()
    {
        if (texture.loadFromFile(path()))
        {
            // texture.setSmooth(true);
            sprite.setTexture(texture);
        }
        else
        {
            // error
            std::cerr << "Error: cannot load " << path() << std::endl;
            texture.loadFromFile("res/tex/default.png");
            sprite.setTexture(texture);
        }
    }

    void SfmlImageFile::renderSprite(UniversalObjectInterface * spriteInfo)
    {
        sprite.setColor(sf::Color(255, 255, 255, 255 * spriteInfo->getAlpha()));
        sprite.setPosition(spriteInfo->getPosition().x, spriteInfo->getPosition().y);
        sprite.setRotation(toDegrees(spriteInfo->getAngle()));
        sprite.setScale(spriteInfo->getScale().x, spriteInfo->getScale().y);
        sprite.setOrigin(spriteInfo->getPivot().x*texture.getSize().x, spriteInfo->getPivot().y*texture.getSize().y);
        renderWindow->draw(sprite);
    }

}
