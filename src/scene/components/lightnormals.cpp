#include "scene/components/lightnormals.hpp"

#include "context/context.hpp"
#include "scene/layer.hpp"
#include "tools/tools.hpp"

using namespace scene;

LightNormals::LightNormals(Entity& entity)
    : baseClass(entity)
{
}

LightNormals::~LightNormals()
{
}

//-------------------//
//----- Routine -----//

void LightNormals::update(const sf::Time& dt)
{
    // TODO The current way the minimap works force us to use such a variable
    m_drawn = false;
}

//---------------------//
//----- Callbacks -----//

void LightNormals::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (m_drawn);
    const_cast<bool&>(m_drawn) = true;

    states.shader = nullptr;
    m_lightSystem->normalsTargetDraw(m_shape, states);
}

void LightNormals::draw(sf::RenderTarget& target, sf::RenderStates states, const sf::FloatRect& clipArea) const
{
    // TODO
}

void LightNormals::onSizeChanged()
{
    m_shape.setSize(m_entity.size());
}

void LightNormals::onLayerChanged(Layer* layer)
{
    if (layer == nullptr) {
        m_lightSystem = nullptr;
        return;
    }

    m_lightSystem = &layer->lightSystem();
}

//---------------------------//
//----- Normals control -----//

void LightNormals::setNormalsTexture(const std::string& textureID)
{
    m_shape.setTexture(&context::context.textures.get(textureID));
}
