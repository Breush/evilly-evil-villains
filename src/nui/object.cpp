#include "core/application.hpp"
#include "nui/object.hpp"
#include "resources/holder.hpp"
#include "nui/uicore.hpp"

#include <SFML/OpenGL.hpp>

using namespace nui;

Object::Object()
    : m_core(nullptr)
    , m_centered(false)
    , m_focusable(true)
    , m_visible(true)
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
    for (auto& part : m_parts)
        returnif (part.drawable == drawable);

    m_parts.push_back({drawable, shader, nullptr});
}

void Object::removePart(sf::Drawable* drawable)
{
    for (uint i = 0; i < m_parts.size(); ++i) {
        if (m_parts[i].drawable == drawable) {
            m_parts.erase(m_parts.begin() + i);
            return;
        }
    }
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

void Object::changedFocusRect()
{
    setStatus(true);
}

void Object::changedVisible()
{
    setDetectable(m_visible);
    setStatus(true);
}

//-------------------//
//----- Drawing -----//

void Object::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const sf::Shader* shader = states.shader;

    // Transform from sf::Tranformable
    states.transform = getTransform();

    // Drawing parts
    for (auto& part : m_parts)
    {
        // Setting shader if needed
        if (part.shader != Shaders::NONE)
            states.shader = &Application::context().shaders.get(part.shader);
        else
            states.shader = shader;

        // Clipping if needed
        if (part.clippingRect != nullptr) {
            glEnable(GL_SCISSOR_TEST);

            sf::FloatRect r(*part.clippingRect);
            r = states.transform.transformRect(r);

            const auto& screenSize = Application::context().screenSize;
            const auto& resolution = Application::context().resolution;
            const auto& effectiveDisplay = Application::context().effectiveDisplay;

            auto halfGap = (screenSize - effectiveDisplay) / 2.f;
            auto viewRatio = sf::vdiv(effectiveDisplay, resolution);

            // Correct position and ratio SFML <-> GLSL
            r.top = resolution.y - r.height - r.top;
            sf::rmulin(r, viewRatio);
            sf::raddin(r, halfGap);

            glScissor(r.left, r.top, r.width, r.height);
        }

        target.draw(*part.drawable, states);

        // End of clipping
        if (part.clippingRect != nullptr)
            glDisable(GL_SCISSOR_TEST);
    }
}
