#include "spriter/sfmlimagefile.hpp"

#include "context/context.hpp"

#include <Spriter/objectinfo/universalobjectinterface.h>

#include <iostream>

using namespace SpriterEngine;

SfmlImageFile::SfmlImageFile(std::string filePath, point defaultPivot)
    : baseClass(filePath, defaultPivot)
{
    auto fileID = context::context.textures.getID(path());
    auto& texture = context::context.textures.get(fileID);
    sprite.setTexture(texture);
    m_textureSize = texture.getSize();
}

//-------------------//
//----- Routine -----//

void SfmlImageFile::renderSprite(UniversalObjectInterface *spriteInfo, sf::RenderTarget& target, sf::RenderStates& states, const sf::Color& tiltColor)
{
    auto color = tiltColor;
    color.a *= spriteInfo->getAlpha();

    sprite.setColor(color);
    sprite.setPosition(spriteInfo->getPosition().x, spriteInfo->getPosition().y);
    sprite.setRotation(toDegrees(spriteInfo->getAngle()));
    sprite.setScale(spriteInfo->getScale().x, spriteInfo->getScale().y);
    sprite.setOrigin(spriteInfo->getPivot().x * m_textureSize.x, spriteInfo->getPivot().y * m_textureSize.y);

    target.draw(sprite, states);
}
