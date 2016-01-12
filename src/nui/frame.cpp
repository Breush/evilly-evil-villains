#include "nui/frame.hpp"

#include "core/application.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

using namespace nui;

Frame::Frame()
{
    setDetectable(false);

    // Corners
    addPart(&m_topLeft);
    addPart(&m_topRight);
    addPart(&m_bottomLeft);
    addPart(&m_bottomRight);

    const auto& topLeftTexture = Application::context().textures.get("nui/frame/top_left");

    m_topLeft.setTexture(topLeftTexture);
    m_topRight.setTexture(topLeftTexture);
    m_bottomLeft.setTexture(topLeftTexture);
    m_bottomRight.setTexture(topLeftTexture);

    m_topRight.setScale(-1.f, 1.f);
    m_bottomLeft.setScale(1.f, -1.f);
    m_bottomRight.setScale(-1.f, -1.f);

    m_cornerSize = sf::v2f(topLeftTexture.getSize());

    // Borders
    addPart(&m_top);
    addPart(&m_left);
    addPart(&m_right);
    addPart(&m_bottom);

    const auto& topTexture = Application::context().textures.get("nui/frame/top");
    const auto& leftTexture = Application::context().textures.get("nui/frame/left");

    m_top.setTexture(&topTexture);
    m_left.setTexture(&leftTexture);
    m_right.setTexture(&leftTexture);
    m_bottom.setTexture(&topTexture);

    m_right.setScale(-1.f, 1.f);
    m_bottom.setScale(1.f, -1.f);

    // Fill
    addPart(&m_fill);

    const auto& fillTexture = Application::context().textures.get("nui/frame/fill");

    m_fill.setTexture(&fillTexture);

    // Title
    m_titleText.setFont(Application::context().fonts.get("nui"));
    m_titleText.setColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    const auto& titleLeftTexture = Application::context().textures.get("nui/frame/title_left");
    const auto& titleMiddleTexture = Application::context().textures.get("nui/frame/title_middle");
    const auto& titleRightTexture = Application::context().textures.get("nui/frame/title_right");

    m_titleLeft.setTexture(&titleLeftTexture);
    m_titleMiddle.setTexture(&titleMiddleTexture);
    m_titleRight.setTexture(&titleRightTexture);

    m_titleLeftWidth = static_cast<float>(titleLeftTexture.getSize().x);
    m_titleRightWidth = static_cast<float>(titleRightTexture.getSize().x);

    m_titleMiddle.setOrigin(-m_titleLeftWidth, 0.f);
    m_titleRight.setOrigin(m_titleRightWidth, 0.f);
}

//-------------------//
//----- Routine -----//

void Frame::onSizeChanges()
{
    refresh();
}

void Frame::onChildDetached(scene::Entity&)
{
    m_content = nullptr;
    updateSize();
}

void Frame::onChildSizeChanges(scene::Entity& child)
{
    returnif (m_content != &child);
    updateSize();
}

void Frame::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_hPadding = cNUI.hPadding * 2.f;
    m_vPadding = cNUI.vPadding / 2.f;
    m_fontSize = cNUI.fontSize;

    m_titleText.setCharacterSize(m_fontSize);

    refreshTitle();
}

void Frame::updateSize()
{
    if (m_content == nullptr)
        setSize(2.f * m_cornerSize);
    else
        setSize(m_content->size() + 2.f * m_cornerSize);
}

//------------------------------//
//----- Content management -----//

void Frame::setContent(scene::Entity& entity)
{
    if (m_content != nullptr) detachChild(*m_content);
    m_content = &entity;
    attachChild(*m_content);
    updateSize();
}

void Frame::setTitle(const std::wstring& title)
{
    // Remove previous title if any
    if (!m_titleText.getString().isEmpty()) {
        removePart(&m_titleLeft);
        removePart(&m_titleMiddle);
        removePart(&m_titleRight);
        removePart(&m_titleText);
    }

    m_titleText.setString(title);

    // Add title if any
    if (!m_titleText.getString().isEmpty()) {
        addPart(&m_titleLeft);
        addPart(&m_titleMiddle);
        addPart(&m_titleRight);
        addPart(&m_titleText);
    }

    refreshTitle();
}

//---------------//
//----- ICU -----//

void Frame::refresh()
{
    refreshTitle();
    refreshFrame();
}

void Frame::refreshFrame()
{
    m_topLeft.setPosition(0.f, 0.f);
    m_topRight.setPosition(size().x, 0.f);
    m_bottomLeft.setPosition(0.f, size().y);
    m_bottomRight.setPosition(size().x, size().y);

    m_top.setPosition(m_cornerSize.x, 0.f);
    m_left.setPosition(0.f, m_cornerSize.y);
    m_right.setPosition(size().x, m_cornerSize.y);
    m_bottom.setPosition(m_cornerSize.x, size().y);

    m_fill.setPosition(m_cornerSize);

    m_top.setSize({size().x - 2.f * m_cornerSize.x, m_cornerSize.y});
    m_left.setSize({m_cornerSize.x, size().y - 2.f * m_cornerSize.y});
    m_right.setSize({m_cornerSize.x, size().y - 2.f * m_cornerSize.y});
    m_bottom.setSize({size().x - 2.f * m_cornerSize.x, m_cornerSize.y});

    m_fill.setSize(size() - 2.f * m_cornerSize);

    returnif (m_content == nullptr);

    m_content->setLocalPosition(m_cornerSize);
}

void Frame::refreshTitle()
{
    returnif (m_titleText.getString().isEmpty());

    sf::Vector2f titleSize;
    auto textWidth = boundsSize(m_titleText).x;
    titleSize.x = textWidth + 2.f * m_hPadding;
    titleSize.y = m_fontSize + 2.f * m_vPadding;
    titleSize.x = std::max(5.f * m_hPadding, titleSize.x);

    sf::Vector2f titleOffset;
    titleOffset.x = m_hPadding;
    titleOffset.y = -0.3f * titleSize.y;

    m_titleText.setOrigin(0.5f * textWidth, m_fontSize);
    m_titleText.setPosition(titleOffset.x + 0.5f * titleSize.x, titleOffset.y + m_vPadding + 0.75f * m_fontSize);

    m_titleLeft.setPosition(titleOffset);
    m_titleMiddle.setPosition(titleOffset);
    m_titleRight.setPosition(titleOffset.x + titleSize.x, titleOffset.y);

    m_titleLeft.setSize({m_titleLeftWidth, titleSize.y});
    m_titleMiddle.setSize({titleSize.x - (m_titleLeftWidth + m_titleRightWidth), titleSize.y});
    m_titleRight.setSize({m_titleRightWidth, titleSize.y});
}
