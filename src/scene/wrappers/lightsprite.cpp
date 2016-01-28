#include "scene/wrappers/lightsprite.hpp"

#include "scene/components/lightnormals.hpp"
#include "context/context.hpp"

using namespace scene;

LightSprite::LightSprite()
{
    addPart(&m_shape);
}

LightSprite::~LightSprite()
{
    removeComponent<scene::LightNormals>();
}

//-------------------//
//----- Routine -----//

void LightSprite::onSizeChanges()
{
    m_shape.setSize(size());
}

void LightSprite::onScaleChanges()
{
    m_shape.setScale(scale());
}

//--------------------//
//----- Wrappers -----//

void LightSprite::setTexture(const std::string& textureID)
{
    m_shape.setTexture(&context::context.textures.get(textureID));

    auto normalsTextureID = textureID + "_NORMALS";
    if (context::context.textures.stored(normalsTextureID)) {
        auto lightNormalsComponent = getComponent<scene::LightNormals>();
        if (lightNormalsComponent == nullptr)
            lightNormalsComponent = addComponent<scene::LightNormals>(*this);
        lightNormalsComponent->setNormalsTexture(normalsTextureID);
    }
}
