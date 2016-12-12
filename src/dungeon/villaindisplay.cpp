#include "dungeon/villaindisplay.hpp"

#include "core/gettext.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <cmath>

using namespace dungeon;

VillainDisplay::VillainDisplay()
{
    // Background
    addPart(&m_background);
    m_background.setFillColor({255u, 255u, 255u, 120u});
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(1.f);
    m_background.setPosition({1.f, 1.f});

    // Image
    attachChild(m_image);
    m_image.setRelativePosition({0.5f, 1.f});
    m_image.setRelativeOrigin({0.5f, 1.f});
    m_image.setDepth3D(5.f);

    // Villain name
    attachChild(m_villainName);
    m_villainName.setRelativePosition({0.5f, 0.f});
    m_villainName.setRelativeOrigin({0.5f, 0.f});
    m_villainName.setPrestyle(scene::RichLabel::Prestyle::NUI_TITLE);
    m_villainName.setText(_("NEW"));
}

//-------------------//
//----- Routine -----//

void VillainDisplay::onSizeChanges()
{
    m_background.setSize(size() - 2.f);
    refreshImageSize();
}

void VillainDisplay::updateRoutine(const sf::Time& dt)
{
    returnif (!m_hovered && !m_selected);

    m_yawTime += dt.asSeconds();
    float yaw = 45.f * std::sin(m_yawTime);
    m_image.setYaw(yaw);
}

//------------------//
//----- Events -----//

bool VillainDisplay::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    if (m_clickCallback != nullptr) {
        m_clickCallback(*this);
        return true;
    }
    return false;
}

bool VillainDisplay::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (m_hovered) true;
    m_hovered = true;
    return false;
}

void VillainDisplay::handleMouseLeft()
{
    m_hovered = false;
}

//-------------------//
//----- Control -----//

void VillainDisplay::setSource(const context::Villain& villain)
{
    m_source = &villain;
    refreshFromSource();
}

void VillainDisplay::setSelected(bool selected)
{
    returnif (m_selected == selected);
    m_selected = selected;
    refreshFromSource();
}

//---------------//
//----- ICU -----//

void VillainDisplay::refreshImageSize()
{
    auto texture = m_image.getTexture();
    returnif (texture == nullptr);

    auto textureSize = sf::v2f(texture->getSize());
    auto scaleFactor = size().x / textureSize.x;

    m_image.setSize(textureSize);
    m_image.setScale({scaleFactor, scaleFactor});
    m_image.setOrigin3D({0.5f * textureSize.x, 0.5f * textureSize.y, 0.f});
}

void VillainDisplay::refreshFromSource()
{
    returnif (m_source == nullptr);

    m_villainName.setText(m_source->name);

    if (m_source->man) {
        m_background.setFillColor(m_selected? sf::Color{170u, 151u, 184u, 240u} : sf::Color{170u, 151u, 184u, 200u});
        m_image.setTexture("core/villains/man");
    }
    else {
        m_background.setFillColor(m_selected? sf::Color{184u, 171u, 151u, 240u} : sf::Color{184u, 171u, 151u, 200u});
        m_image.setTexture("core/villains/woman");
    }

    m_background.setOutlineThickness(m_selected? 3.f : 1.f);
    m_villainName.setShader(m_selected? "core/nui/select/select" : "");

    refreshImageSize();
}
