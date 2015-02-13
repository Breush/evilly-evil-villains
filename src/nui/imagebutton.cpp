#include "nui/imagebutton.hpp"

#include "core/application.hpp"

using namespace nui;

ImageButton::ImageButton()
{
    // Display style
    setShowLines(false);
}

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
    const sf::Vector2f offset((size().x - maxTextSize().x) / 2.f, m_imageSize.y);
    text().move(offset);
    if (showLines()) {
        topLine().move(offset);
        botLine().move(offset);
    }
}

//-------------------//
//----- Changes -----//

void ImageButton::updateSize()
{
    updateButtonSize();
    const auto& buttonDimensions = buttonSize();
    auto width = std::max(buttonDimensions.x, m_imageSize.x);
    setSize({width, buttonDimensions.y + m_imageSize.y});
}

//------------------------//
//----- Mouse events -----//

void ImageButton::handleMouseMoved(const sf::Vector2f& mousePos)
{
    // From button
    baseClass::handleMouseMoved(mousePos);

    setPartShader(&m_image, Shaders::NUI_HOVER);
}

//------------------//
//----- Action -----//

void ImageButton::setImage(Textures::ID imageID)
{
    m_image.setTexture(Application::context().textures.get(imageID));
}

void ImageButton::setVisual(const std::wstring& text, Textures::ID imageID, const sf::Vector2f& inImageSize)
{
    setText(text);
    setImage(imageID);
    setImageSize(inImageSize);
}
