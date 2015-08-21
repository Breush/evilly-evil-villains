#include "nui/imagebutton.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"

using namespace nui;

ImageButton::ImageButton()
{
    // Display style
    showLines(false);

    // Image
    addPart(&m_image);
}

//-------------------//
//----- Routine -----//

void ImageButton::onSizeChanges()
{
    // From button, will refresh all positions
    baseClass::onSizeChanges();

    // Image
    if (m_image.getTexture() != nullptr) {
        const auto& textureSize = m_image.getTexture()->getSize();
        m_image.setScale(m_imageSize / sf::v2f(textureSize));
        m_image.setPosition({(size().x - m_imageSize.x) / 2.f, 0.f});
    }

    // Re-positioning
    const sf::Vector2f offset((size().x - maxTextSize().x) / 2.f, m_imageSize.y + m_vPadding);
    text().move(offset);
    if (linesShowed()) {
        topLine().move(offset);
        botLine().move(offset);
    }
}

void ImageButton::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_imageSize = {cNUI.hintImageSide, cNUI.hintImageSide};
    m_vPadding = cNUI.vPadding;
}

//-------------------//
//----- Changes -----//

void ImageButton::updateSize()
{
    updateButtonSize();

    const auto& buttonDimensions = buttonSize();
    auto width = std::max(buttonDimensions.x, m_imageSize.x);
    auto height = buttonDimensions.y + m_imageSize.y + m_vPadding;
    setSize({width, height});
}

//------------------------//
//----- Mouse events -----//

void ImageButton::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // From button
    baseClass::handleMouseMoved(mousePos, nuiPos);

    setPartShader(&m_image, "nui/hover");
}

//------------------//
//----- Action -----//

void ImageButton::setImage(const std::string& imageID)
{
    m_image.setTexture(Application::context().textures.get(imageID));
    updateSize();
}

void ImageButton::setVisual(const std::wstring& text, const std::string& imageID)
{
    setText(text);
    setImage(imageID);
}
