#include "nui/imagebutton.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "config/nui.hpp"

using namespace nui;

ImageButton::ImageButton()
{
    // Display style
    setShowLines(false);

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void ImageButton::update()
{
    // From button
    baseClass::update();

    returnif (m_image.getTexture() == nullptr);

    // Image
    if (m_image.getTexture() != nullptr) {
        const auto& textureSize = m_image.getTexture()->getSize();
        m_image.setScale({m_imageSize.x / textureSize.x, m_imageSize.y / textureSize.y});
        m_image.setPosition({(size().x - m_imageSize.x) / 2.f, 0.f});
        addPart(&m_image);
    }

    // Re-positioning
    const sf::Vector2f offset((size().x - maxTextSize().x) / 2.f, m_imageSize.y + m_vPadding);
    text().move(offset);
    if (showLines()) {
        topLine().move(offset);
        botLine().move(offset);
    }
}

void ImageButton::refreshDisplay()
{
    config::NUI cNUI;

    m_imageSize = {cNUI.hintImageSide, cNUI.hintImageSide};
    m_vPadding = cNUI.vPadding;

    updateSize();
}

//-------------------//
//----- Changes -----//

void ImageButton::updateSize()
{
    updateButtonSize();

    const auto& buttonDimensions = buttonSize();
    auto width = std::max(buttonDimensions.x, m_imageSize.x);
    setSize({width, buttonDimensions.y + m_imageSize.y + m_vPadding});

    update();
}

//------------------------//
//----- Mouse events -----//

void ImageButton::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // From button
    baseClass::handleMouseMoved(mousePos, nuiPos);

    setPartShader(&m_image, ShaderID::NUI_HOVER);
}

//------------------//
//----- Action -----//

void ImageButton::setImage(TextureID imageID)
{
    m_image.setTexture(Application::context().textures.get(imageID));
}

void ImageButton::setVisual(const std::wstring& text, TextureID imageID)
{
    setText(text);
    setImage(imageID);
    updateSize();
}
