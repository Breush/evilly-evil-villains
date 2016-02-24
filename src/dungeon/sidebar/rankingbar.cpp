#include "dungeon/sidebar/rankingbar.hpp"

#include "context/context.hpp"

using namespace dungeon;

RankingBar::RankingBar()
{
    // Progression
    addPart(&m_progressionLeft);
    addPart(&m_progressionRight);
    addPart(&m_progressionMiddle);
    addPart(&m_progressionFill);

    // Content
    addPart(&m_text);
    addPart(&m_logo);

    m_text.setFont(context::context.fonts.get("core/global/fonts/nui"));
}

void RankingBar::init()
{
    const auto& progressionLeftTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/background_left");
    const auto& progressionMiddleTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/background_middle");
    const auto& progressionRightTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/background_right");
    const auto& progressionFillTexture = context::context.textures.get("core/dungeon/sidebar/summary/bar/progression_fill");

    m_progressionLeft.setTexture(&progressionLeftTexture);
    m_progressionMiddle.setTexture(&progressionMiddleTexture);
    m_progressionRight.setTexture(&progressionRightTexture);
    m_progressionFill.setTexture(&progressionFillTexture);

    m_progressionLeftWidth = progressionLeftTexture.getSize().x;
    m_progressionRightWidth = progressionRightTexture.getSize().x;
    m_progressionFillTextureSize = progressionFillTexture.getSize();

    m_progressionRight.setOrigin(m_progressionRightWidth, 0.f);

    refresh();
}

//-------------------//
//----- Routine -----//

void RankingBar::onSizeChanges()
{
    refresh();
}

void RankingBar::refreshNUI(const config::NUIGuides& cNUI)
{
    m_logoSide = 0.8f * cNUI.fontVSpace;
    m_fontSize = 0.8f * cNUI.fontSize;
    m_hPadding = 0.25f * cNUI.hPadding;
    m_vPadding = 0.2f * cNUI.vPadding;

    m_height = 2.f * m_vPadding + m_logoSide;

    // Text
    m_text.setCharacterSize(m_fontSize);

    updateSize();
}

void RankingBar::updateSize()
{
    setSize({m_width, m_height});
}

//-------------------//
//----- Control -----//

void RankingBar::setText(const std::wstring& text)
{
    m_text.setString(text);
}

void RankingBar::setLogo(const std::string& textureID)
{
    m_logo.setTexture(textureID.empty()? nullptr : &context::context.textures.get(textureID));
}

void RankingBar::setTextColor(const sf::Color& color)
{
    m_text.setFillColor(color);
}

void RankingBar::setProgressionColor(const sf::Color& color)
{
    m_progressionFill.setFillColor(color);
}

void RankingBar::setPercent(float percent)
{
    m_percent = percent;
    refreshProgression();
}

void RankingBar::setWidth(float width)
{
    m_width = width;
    updateSize();
}

//---------------//
//----- ICU -----//

void RankingBar::refresh()
{
    // Text
    m_text.setPosition(m_hPadding, m_vPadding);

    // Logo
    float offset = 0.5f * size().x;
    m_logo.setPosition({offset, m_vPadding});
    m_logo.setSize({m_logoSide, m_logoSide});

    // Progression
    offset += m_logoSide + m_hPadding;
    m_progressionFill.setPosition(offset, 0.f);
    m_progressionLeft.setPosition(offset, 0.f);
    m_progressionMiddle.setPosition(offset + m_progressionLeftWidth, 0.f);
    m_progressionRight.setPosition(size().x, 0.f);

    m_progressionWidth = size().x - offset;
    m_progressionLeft.setSize({m_progressionLeftWidth, size().y});
    m_progressionMiddle.setSize({m_progressionWidth - (m_progressionLeftWidth + m_progressionRightWidth), size().y});
    m_progressionRight.setSize({m_progressionRightWidth, size().y});

    refreshProgression();
}

void RankingBar::refreshProgression()
{
    auto fillWidth = m_percent * m_progressionWidth;
    m_progressionFill.setSize({fillWidth, size().y});
    m_progressionFill.setTextureRect({0, 0, static_cast<int>(m_percent * m_progressionFillTextureSize.x), m_progressionFillTextureSize.y});
}
