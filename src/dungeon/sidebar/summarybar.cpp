#include "dungeon/sidebar/summarybar.hpp"

#include "context/context.hpp"

using namespace dungeon;

SummaryBar::SummaryBar()
{
    // Background
    addPart(&m_backgroundLeft);
    addPart(&m_backgroundRight);
    addPart(&m_backgroundMiddle);

    // Content
    addPart(&m_text);
    addPart(&m_logo);

    m_text.setFont(context::context.fonts.get("core/global/fonts/nui"));
}

void SummaryBar::init()
{
    const auto& backgroundLeftTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/background_left");
    const auto& backgroundMiddleTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/background_middle");
    const auto& backgroundRightTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/background_right");

    m_backgroundLeft.setTexture(&backgroundLeftTexture);
    m_backgroundMiddle.setTexture(&backgroundMiddleTexture);
    m_backgroundRight.setTexture(&backgroundRightTexture);

    m_backgroundLeftWidth = backgroundLeftTexture.getSize().x;
    m_backgroundRightWidth = backgroundRightTexture.getSize().x;

    m_backgroundRight.setOrigin(m_backgroundRightWidth, 0.f);

    refresh();
}

//-------------------//
//----- Routine -----//

void SummaryBar::onSizeChanges()
{
    refresh();
}

void SummaryBar::refreshNUI(const config::NUIGuides& cNUI)
{
    m_logoSide = 0.9f * cNUI.fontVSpace;
    m_fontSize = 0.9f * cNUI.fontSize;
    m_hPadding = cNUI.hPadding;
    m_vPadding = 0.5f * cNUI.vPadding;

    m_height = 2.f * m_vPadding + m_logoSide;

    // Text
    m_text.setCharacterSize(m_fontSize);

    updateSize();
}

void SummaryBar::updateSize()
{
    setSize({m_width, m_height});
}

//-------------------//
//----- Control -----//

void SummaryBar::setText(const std::wstring& text)
{
    m_text.setString(text);
}

void SummaryBar::setLogo(const std::string& textureID)
{
    m_logo.setTexture(textureID.empty()? nullptr : &context::context.textures.get(textureID));
}

void SummaryBar::setTextColor(const sf::Color& color)
{
    m_text.setFillColor(color);
}

void SummaryBar::setWidth(float width)
{
    m_width = width;
    updateSize();
}

//---------------//
//----- ICU -----//

void SummaryBar::refresh()
{
    // Background
    m_backgroundLeft.setPosition(0.f, 0.f);
    m_backgroundMiddle.setPosition(m_backgroundLeftWidth, 0.f);
    m_backgroundRight.setPosition(size().x, 0.f);

    m_backgroundLeft.setSize({m_backgroundLeftWidth, size().y});
    m_backgroundMiddle.setSize({size().x - (m_backgroundLeftWidth + m_backgroundRightWidth), size().y});
    m_backgroundRight.setSize({m_backgroundRightWidth, size().y});

    // Text
    m_text.setPosition(m_logoSide + 2.f * m_hPadding, m_vPadding);

    // Logo
    m_logo.setPosition({m_hPadding, m_vPadding});
    m_logo.setSize({m_logoSide, m_logoSide});
}
