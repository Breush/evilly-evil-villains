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

    const auto& topLeftTexture = Application::context().textures.get("core/nui/frame/top_left");

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

    const auto& topTexture = Application::context().textures.get("core/nui/frame/top");
    const auto& leftTexture = Application::context().textures.get("core/nui/frame/left");

    m_top.setTexture(&topTexture);
    m_left.setTexture(&leftTexture);
    m_right.setTexture(&leftTexture);
    m_bottom.setTexture(&topTexture);

    m_right.setScale(-1.f, 1.f);
    m_bottom.setScale(1.f, -1.f);

    // Fill
    addPart(&m_fill);

    const auto& fillTexture = Application::context().textures.get("core/nui/frame/fill");

    m_fill.setTexture(&fillTexture);

    // Title
    m_titleText.setFont(Application::context().fonts.get("core/global/fonts/nui"));
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    const auto& titleLeftTexture = Application::context().textures.get("core/nui/frame/title_left");
    const auto& titleMiddleTexture = Application::context().textures.get("core/nui/frame/title_middle");
    const auto& titleRightTexture = Application::context().textures.get("core/nui/frame/title_right");

    m_titleLeft.setTexture(&titleLeftTexture);
    m_titleMiddle.setTexture(&titleMiddleTexture);
    m_titleRight.setTexture(&titleRightTexture);

    m_titleLeftWidth = static_cast<float>(titleLeftTexture.getSize().x);
    m_titleRightWidth = static_cast<float>(titleRightTexture.getSize().x);

    m_titleRight.setOrigin(m_titleRightWidth, 0.f);
}

//-------------------//
//----- Routine -----//

void Frame::onChildDetached(scene::Entity& child)
{
    returnif (m_content != &child);

    m_content = nullptr;
    updateSize();
}

void Frame::onChildSizeChanges(scene::Entity& child)
{
    returnif (m_content != &child);

    if (m_contentSizeChangedCallback != nullptr)
        m_contentSizeChangedCallback();

    returnif (m_forceContentSize);

    updateSize();
}

void Frame::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_hPadding = cNUI.hPadding * 2.f;
    m_vPadding = cNUI.vPadding * 2.f;
    m_fontSize = cNUI.fontSize;

    m_titleHeight = m_fontSize + 0.5f * m_vPadding;
    m_titleOffset.x = m_hPadding;
    m_titleOffset.y = 0.4f * m_titleHeight;

    m_titleText.setCharacterSize(m_fontSize);

    refreshTitle();
}

void Frame::updateSize()
{
    // Fit size
    if (m_fitSize.x >= 0.f && m_fitSize.y >= 0.f) {
        setSize(m_fitSize);
    }

    // Auto size
    else {
        sf::Vector2f newSize;
        newSize.x = 2.f * m_hPadding;
        newSize.y = 2.f * m_vPadding;

        if (!m_titleText.getString().isEmpty())
            newSize.y += m_titleOffset.y;

        if (m_content != nullptr)
            newSize += m_content->size();

        setSize(newSize);
    }

    refresh();
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

    updateSize();
}

void Frame::setContentSizeChangedCallback(ContentSizeChangedCallback contentSizeChangedCallback)
{
    m_contentSizeChangedCallback = contentSizeChangedCallback;
}

//-------------------//
//----- Display -----//

void Frame::setFitSize(const sf::Vector2f& fitSize, bool forceContentSize)
{
    m_fitSize = fitSize;
    m_forceContentSize = forceContentSize;
    updateSize();
}

//---------------//
//----- ICU -----//

void Frame::refresh()
{
    refreshTitle();
    refreshFrame();
    refreshContent();
}

void Frame::refreshFrame()
{
    float frameOffsetY = 0.f;
    if (!m_titleText.getString().isEmpty())
        frameOffsetY += m_titleOffset.y;

    m_topLeft.setPosition(0.f, frameOffsetY);
    m_topRight.setPosition(size().x, frameOffsetY);
    m_bottomLeft.setPosition(0.f, size().y);
    m_bottomRight.setPosition(size().x, size().y);

    float fillWidth = size().x - 2.f * m_cornerSize.x;
    float fillHeight = size().y - 2.f * m_cornerSize.y - frameOffsetY;

    m_fill.setPosition(m_cornerSize.x, m_cornerSize.y + frameOffsetY);
    m_fill.setSize({fillWidth, fillHeight});

    m_top.setPosition(m_cornerSize.x, frameOffsetY);
    m_left.setPosition(0.f, m_cornerSize.y + frameOffsetY);
    m_right.setPosition(size().x, m_cornerSize.y + frameOffsetY);
    m_bottom.setPosition(m_cornerSize.x, size().y);

    m_top.setSize({fillWidth, m_cornerSize.y});
    m_left.setSize({m_cornerSize.x, fillHeight});
    m_right.setSize({m_cornerSize.x, fillHeight});
    m_bottom.setSize({fillWidth, m_cornerSize.y});
}

void Frame::refreshTitle()
{
    returnif (m_titleText.getString().isEmpty());

    auto textWidth = boundsSize(m_titleText).x;
    float titleWidth = textWidth + 2.f * m_hPadding;
    titleWidth = std::max(5.f * m_hPadding, titleWidth);

    float titleVPadding = 0.5f * (m_titleHeight - m_fontSize);
    m_titleText.setOrigin(0.5f * textWidth, 0.1f * m_fontSize);
    m_titleText.setPosition(m_titleOffset.x + 0.5f * titleWidth, titleVPadding);

    m_titleLeft.setPosition(m_titleOffset.x, 0.f);
    m_titleMiddle.setPosition(m_titleOffset.x + m_titleLeftWidth, 0.f);
    m_titleRight.setPosition(m_titleOffset.x + titleWidth, 0.f);

    m_titleLeft.setSize({m_titleLeftWidth, m_titleHeight});
    m_titleMiddle.setSize({titleWidth - (m_titleLeftWidth + m_titleRightWidth), m_titleHeight});
    m_titleRight.setSize({m_titleRightWidth, m_titleHeight});
}

void Frame::refreshContent()
{
    returnif (m_content == nullptr);

    float frameOffsetY = 0.f;
    if (!m_titleText.getString().isEmpty())
        frameOffsetY += m_titleOffset.y;

    m_content->setLocalPosition({m_hPadding, m_vPadding + frameOffsetY});

    returnif (!m_forceContentSize);

    auto contentSize = size();
    contentSize.x -= 2.f * m_hPadding;
    contentSize.y -= 2.f * m_vPadding - frameOffsetY;
    m_content->setSize(contentSize);
}
