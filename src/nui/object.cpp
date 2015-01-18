#include "nui/object.hpp"

#include "resources/holder.hpp"
#include "nui/uicore.hpp"

#include <SFML/OpenGL.hpp>

using namespace nui;

Object::Object()
    : m_core(nullptr)
    , m_status(true)
    , m_centered(false)
    , m_focusable(true)
    , m_detectable(true)
    , m_parent(nullptr)
{
}

//-----------------//
//----- Parts -----//

void Object::clearParts()
{
    m_parts.clear();
}

void Object::addPart(sf::Drawable* drawable, Shaders::ID shader)
{
    m_parts.push_back({drawable, shader, nullptr});
}

void Object::setPartShader(sf::Drawable* drawable, Shaders::ID shader)
{
    for (auto& part : m_parts)
    {
        if (part.drawable == drawable)
        {
            part.shader = shader;
            return;
        }
    }
}

void Object::setPartClippingRect(sf::Drawable* drawable, sf::IntRect* clippingRect)
{
    for (auto& part : m_parts)
    {
        if (part.drawable == drawable)
        {
            part.clippingRect = clippingRect;
            return;
        }
    }
}

void Object::resetPartsShader()
{
    for (auto& part : m_parts)
        part.shader = Shaders::NONE;
}

//-------------------//
//----- Updates -----//

void Object::changedSize()
{
    if (centered())
        setOrigin(0.5f * size());

    if (!ownsFocus())
        setFocusRect({0.f, 0.f, size().x, size().y});

    setStatus(true);
}

void Object::changedParent()
{
    setStatus(true);
}

void Object::changedFocusRect()
{
    setStatus(true);
}

void Object::changedChild(Object*)
{
}

//-------------------//
//----- Drawing -----//

void Object::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const sf::Shader* shader = states.shader;

    // Transform from sf::Tranformable
    states.transform *= getTransform();

    // Drawing parts
    for (auto& part : m_parts)
    {
        // Setting shader if needed
        if (part.shader != Shaders::NONE)
            states.shader = &core()->context().shaders->get(part.shader);
        else
            states.shader = shader;

        // Clipping if needed
        if (part.clippingRect != nullptr) {
            glEnable(GL_SCISSOR_TEST);
            sf::FloatRect r(*part.clippingRect);
            r = getTransform().transformRect(r);
            glScissor(r.left, core()->viewSize().y - r.height - r.top, r.width, r.height);
        }

        target.draw(*part.drawable, states);

        // End of clipping
        if (part.clippingRect != nullptr)
            glDisable(GL_SCISSOR_TEST);
    }
}

void Object::update(sf::Time)
{
    if (status() && parent() != nullptr)
        parent()->changedChild(this);

    setStatus(false);
}
